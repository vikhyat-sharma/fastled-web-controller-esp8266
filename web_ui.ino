#include <LittleFS.h>
#include <ArduinoJson.h>

// Forward declarations for state defined in fastLED.ino
extern bool saveWifiToEEPROM(const String &newSsid, const String &newPassword);
extern void markSettingsDirty();

// ── Response helpers ──────────────────────────────────────────────────────────

static void sendJson(AsyncWebServerRequest *req, int code, const String &payload) {
  AsyncWebServerResponse *r = req->beginResponse(code, "application/json", payload);
  r->addHeader("Cache-Control", "no-store");
  r->addHeader("X-Content-Type-Options", "nosniff");
  req->send(r);
}

// Wraps payload in {"success":true,"data":{...}}
static void sendOk(AsyncWebServerRequest *req, const String &dataJson) {
  sendJson(req, 200, "{\"success\":true,\"data\":" + dataJson + "}");
}

// {"success":false,"error":{"code":"...","message":"..."}}
static void sendErr(AsyncWebServerRequest *req, int code,
                    const char *errCode, const char *msg) {
  String j = "{\"success\":false,\"error\":{\"code\":\"";
  j += errCode;
  j += "\",\"message\":\"";
  j += msg;
  j += "\"}}";
  sendJson(req, code, j);
}

// ── LittleFS helpers ──────────────────────────────────────────────────────────

static String readTextFile(const String &path) {
  if (!LittleFS.exists(path)) return "";
  File f = LittleFS.open(path, "r");
  if (!f) return "";
  String s = f.readString();
  f.close();
  return s;
}

static bool writeTextFile(const String &path, const String &content) {
  File f = LittleFS.open(path, "w");
  if (!f) return false;
  size_t written = f.print(content);
  f.close();
  return written == content.length();
}

// Sanitise a user-supplied name to [A-Za-z0-9_-], max 32 chars.
static String sanitizeName(const String &input) {
  String v = input;
  v.trim();
  if (v.length() > 32) v = v.substring(0, 32);
  for (size_t i = 0; i < v.length(); i++) {
    char c = v.charAt(i);
    bool ok = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') || c == '_' || c == '-';
    if (!ok) v.setCharAt(i, '_');
  }
  return v.length() > 0 ? v : "default";
}

// Convert comma-separated string to JSON array of strings.
static String csvToJsonArray(const String &csv) {
  String j = "[";
  bool first = true;
  int start = 0;
  while (start <= (int)csv.length()) {
    int comma = csv.indexOf(',', start);
    String tok = (comma == -1) ? csv.substring(start) : csv.substring(start, comma);
    tok.trim();
    if (tok.length() > 0) {
      if (!first) j += ',';
      j += '"';
      j += tok;
      j += '"';
      first = false;
    }
    if (comma == -1) break;
    start = comma + 1;
  }
  j += ']';
  return j;
}

// ── State JSON builder ────────────────────────────────────────────────────────

static String buildStateJson() {
  String j = "{";
  j += "\"pattern\":"    + String(currentPattern)        + ',';
  j += "\"patternName\":\"" + String(patternName(currentPattern)) + "\",";
  j += "\"brightness\":" + String(gBrightness)           + ',';
  j += "\"speed\":"      + String(gSpeed)                + ',';
  j += "\"hue\":"        + String(gHue)                  + ',';
  j += "\"saturation\":" + String(gSat)                  + ',';
  j += "\"autoCycle\":"  + String(autoCycle ? "true" : "false") + ',';
  j += "\"colorOverride\":" + String(useColorPickerOverride ? "true" : "false") + ',';
  j += "\"firmware\":\"" + String(FIRMWARE_VERSION)      + '"';
  j += '}';
  return j;
}

// ── Param helpers ─────────────────────────────────────────────────────────────

// Returns true and sets out if param exists and is a valid integer in [lo,hi].
static bool getIntParam(AsyncWebServerRequest *req, const char *name,
                        int lo, int hi, int &out) {
  if (!req->hasParam(name)) return false;
  String v = req->getParam(name)->value();
  if (v.length() == 0 || v.length() > 6) return false;
  for (char c : v) if (c != '-' && (c < '0' || c > '9')) return false;
  int val = v.toInt();
  if (val < lo || val > hi) return false;
  out = val;
  return true;
}

// ── Main page HTML ────────────────────────────────────────────────────────────
// Served from RAM. Pattern options are injected server-side to avoid a
// round-trip fetch on first load. The rest of the UI uses fetch() for
// live updates so the HTML itself stays small.

static const char PAGE_HTML[] PROGMEM = R"rawhtml(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>FastLED Controller</title>
<style>
:root{--accent:#5c6bc0;--accent2:#7e57c2;--bg:#f0f2f8;--card:#fff;--text:#222;--muted:#666;--border:#dde}
@media(prefers-color-scheme:dark){:root{--bg:#1a1a2e;--card:#16213e;--text:#e0e0e0;--muted:#aaa;--border:#2a2a4a}}
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:var(--bg);color:var(--text);padding:12px;min-height:100vh}
h1{font-size:1.3rem;font-weight:700;background:linear-gradient(135deg,var(--accent),var(--accent2));-webkit-background-clip:text;-webkit-text-fill-color:transparent;margin-bottom:4px}
.sub{font-size:.8rem;color:var(--muted);margin-bottom:16px}
.card{background:var(--card);border-radius:12px;padding:16px;margin-bottom:12px;box-shadow:0 2px 8px rgba(0,0,0,.08)}
.card h2{font-size:.95rem;font-weight:600;margin-bottom:12px;color:var(--accent)}
label{display:block;font-size:.8rem;color:var(--muted);margin-bottom:4px;margin-top:10px}
label:first-of-type{margin-top:0}
input[type=range]{width:100%;accent-color:var(--accent);cursor:pointer}
select{width:100%;padding:8px;border-radius:8px;border:1.5px solid var(--border);background:var(--card);color:var(--text);font-size:.9rem;cursor:pointer}
.btn{display:inline-block;padding:9px 18px;border-radius:8px;border:none;background:linear-gradient(135deg,var(--accent),var(--accent2));color:#fff;font-size:.9rem;font-weight:600;cursor:pointer;transition:opacity .15s}
.btn:hover{opacity:.88}
.btn.secondary{background:var(--border);color:var(--text)}
.btn.danger{background:#e53935;color:#fff}
.btn-row{display:flex;gap:8px;flex-wrap:wrap;margin-top:10px}
.val{font-size:.85rem;font-weight:600;color:var(--accent);margin-left:6px}
#status-bar{display:flex;align-items:center;gap:8px;font-size:.8rem;color:var(--muted);margin-bottom:12px}
.dot{width:8px;height:8px;border-radius:50%;background:#4caf50;flex-shrink:0}
.dot.off{background:#e53935}
.dot.warn{background:#ff9800}
#pattern-name{font-weight:700;color:var(--accent)}
details summary{cursor:pointer;font-size:.85rem;color:var(--accent);font-weight:600;padding:4px 0;user-select:none}
#pattern-list{margin-top:8px;max-height:220px;overflow-y:auto;border:1.5px solid var(--border);border-radius:8px}
.pi{padding:8px 12px;font-size:.85rem;cursor:pointer;border-bottom:1px solid var(--border);transition:background .15s}
.pi:last-child{border-bottom:none}
.pi:hover{background:rgba(92,107,192,.1)}
.pi.active{font-weight:700;color:var(--accent)}
input[type=color]{width:60px;height:36px;border:2px solid var(--border);border-radius:8px;cursor:pointer;padding:2px}
.row{display:flex;align-items:center;gap:10px;flex-wrap:wrap}
.info{font-size:.75rem;color:var(--muted);margin-top:6px}
input[type=text],input[type=password]{width:100%;padding:8px;border-radius:8px;border:1.5px solid var(--border);background:var(--card);color:var(--text);font-size:.9rem;margin-top:4px}
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:#323232;color:#fff;padding:10px 20px;border-radius:8px;font-size:.85rem;opacity:0;transition:opacity .3s;pointer-events:none;z-index:999}
.toast.show{opacity:1}
</style>
</head>
<body>
<h1>FastLED Controller</h1>
<div class="sub">Firmware %FW_VER%</div>

<div id="status-bar">
  <span class="dot" id="conn-dot"></span>
  <span id="conn-label">Connecting...</span>
  &nbsp;|&nbsp;
  Pattern: <span id="pattern-name">—</span>
  &nbsp;|&nbsp;
  Heap: <span id="heap">—</span>
</div>

<div class="card">
  <h2>Pattern</h2>
  <select id="pattern-sel">%PATTERN_OPTIONS%</select>
  <div class="btn-row">
    <button class="btn" onclick="applyPattern()">Apply</button>
    <button class="btn secondary" onclick="nextPattern()">Next ›</button>
  </div>
  <details style="margin-top:12px">
    <summary>Browse all patterns</summary>
    <div id="pattern-list"><em style="padding:8px;display:block;color:var(--muted)">Loading...</em></div>
  </details>
</div>

<div class="card">
  <h2>Color &amp; Brightness</h2>
  <label>Hue <span class="val" id="hue-val">%HUE%</span></label>
  <input type="range" id="hue" min="0" max="255" value="%HUE%" oninput="sliderInput('hue-val',this.value)">
  <label>Saturation <span class="val" id="sat-val">%SAT%</span></label>
  <input type="range" id="sat" min="0" max="255" value="%SAT%" oninput="sliderInput('sat-val',this.value)">
  <label>Brightness <span class="val" id="bright-val">%BRIGHT%</span></label>
  <input type="range" id="bright" min="0" max="255" value="%BRIGHT%" oninput="sliderInput('bright-val',this.value)">
  <div class="btn-row"><button class="btn" onclick="applyHSV()">Apply</button></div>
  <label style="margin-top:14px">Color override</label>
  <div class="row">
    <input type="color" id="color-pick" value="#ff0000">
    <button class="btn" onclick="applyColor()">Apply color</button>
    <button class="btn secondary" onclick="clearColor()">Clear override</button>
  </div>
</div>

<div class="card">
  <h2>Speed</h2>
  <label>Speed (1–100) <span class="val" id="speed-val">%SPEED%</span></label>
  <input type="range" id="speed" min="1" max="100" value="%SPEED%" oninput="sliderInput('speed-val',this.value)">
  <div class="btn-row"><button class="btn" onclick="applySpeed()">Apply</button></div>
</div>

<div class="card">
  <h2>Palette</h2>
  <select id="palette-sel">
    <option value="0">Rainbow</option>
    <option value="1">Party</option>
    <option value="2">Ocean</option>
    <option value="3">Heat</option>
    <option value="4">Lava</option>
  </select>
  <div class="btn-row"><button class="btn" onclick="applyPalette()">Apply</button></div>
</div>

<div class="card">
  <h2>Auto-cycle</h2>
  <p class="info">Automatically advances to the next pattern every ~13 s.</p>
  <div class="btn-row">
    <button class="btn" id="ac-btn" onclick="toggleAutoCycle()">—</button>
  </div>
</div>

<div class="card">
  <h2>OTA Update</h2>
  <p class="info">Flash new firmware over Wi-Fi using Arduino IDE or arduino-cli.<br>
  Hostname: <strong>%HOSTNAME%.local</strong> &nbsp;|&nbsp; Port: 8266</p>
</div>

<div class="card">
  <h2>Wi-Fi Credentials</h2>
  <p class="info">Saved to EEPROM. Device reboots after saving.</p>
  <label>SSID</label>
  <input type="text" id="wifi-ssid" maxlength="31" placeholder="Network name">
  <label>Password</label>
  <input type="password" id="wifi-pw" maxlength="63" placeholder="Password">
  <div class="btn-row">
    <button class="btn danger" onclick="saveWifi()">Save &amp; Reboot</button>
  </div>
</div>

<div class="toast" id="toast"></div>

<script>
var state = {};

function toast(msg, dur) {
  var t = document.getElementById('toast');
  t.textContent = msg;
  t.classList.add('show');
  setTimeout(function(){ t.classList.remove('show'); }, dur || 2500);
}

function sliderInput(id, v) { document.getElementById(id).textContent = v; }

function post(url, body) {
  return fetch(url, {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: body
  }).then(function(r){ return r.json(); });
}

function get(url) { return fetch(url).then(function(r){ return r.json(); }); }

function applyState(s) {
  state = s;
  document.getElementById('pattern-name').textContent = s.patternName || s.pattern;
  document.getElementById('heap').textContent = (s.freeHeap ? Math.round(s.freeHeap/1024)+'KB' : '—');
  document.getElementById('ac-btn').textContent = s.autoCycle ? 'Disable auto-cycle' : 'Enable auto-cycle';
  var sel = document.getElementById('pattern-sel');
  if (sel.value != s.pattern) sel.value = s.pattern;
  document.getElementById('hue').value = s.hue;
  document.getElementById('hue-val').textContent = s.hue;
  document.getElementById('sat').value = s.saturation;
  document.getElementById('sat-val').textContent = s.saturation;
  document.getElementById('bright').value = s.brightness;
  document.getElementById('bright-val').textContent = s.brightness;
  document.getElementById('speed').value = s.speed;
  document.getElementById('speed-val').textContent = s.speed;
}

function refreshStatus() {
  get('/json/status').then(function(r){
    if (r.success) {
      applyState(r.data);
      document.getElementById('conn-dot').className = 'dot';
      document.getElementById('conn-label').textContent = 'Connected';
    }
  }).catch(function(){
    document.getElementById('conn-dot').className = 'dot off';
    document.getElementById('conn-label').textContent = 'Offline';
  });
}

function applyPattern() {
  var idx = document.getElementById('pattern-sel').value;
  post('/json/pattern', 'index='+idx).then(function(r){
    if (r.success) { applyState(r.data); toast('Pattern applied'); }
    else toast(r.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function nextPattern() {
  post('/next', '').then(function(r){
    if (r.success) applyState(r.data);
  }).catch(function(){});
}

function applyHSV() {
  var h = document.getElementById('hue').value;
  var s = document.getElementById('sat').value;
  var v = document.getElementById('bright').value;
  post('/json/color', 'h='+h+'&s='+s+'&v='+v).then(function(r){
    if (r.success) { applyState(r.data); toast('Color applied'); }
    else toast(r.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function applyColor() {
  var hex = document.getElementById('color-pick').value;
  var r = parseInt(hex.substr(1,2),16);
  var g = parseInt(hex.substr(3,2),16);
  var b = parseInt(hex.substr(5,2),16);
  post('/setColor', 'r='+r+'&g='+g+'&b='+b).then(function(res){
    if (res.success) toast('Color override applied');
    else toast(res.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function clearColor() {
  post('/toggleColorMode', '').then(function(r){
    if (r.success) toast('Color override cleared');
  }).catch(function(){});
}

function applySpeed() {
  var s = document.getElementById('speed').value;
  post('/json/speed', 's='+s).then(function(r){
    if (r.success) { applyState(r.data); toast('Speed applied'); }
    else toast(r.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function applyPalette() {
  var p = document.getElementById('palette-sel').value;
  post('/palette', 'p='+p).then(function(r){
    if (r.success) toast('Palette applied');
    else toast(r.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function toggleAutoCycle() {
  var next = state.autoCycle ? '0' : '1';
  post('/json/auto-cycle', 'state='+next).then(function(r){
    if (r.success) { applyState(r.data); toast('Auto-cycle updated'); }
    else toast(r.error.message);
  }).catch(function(){ toast('Request failed'); });
}

function saveWifi() {
  var s = document.getElementById('wifi-ssid').value.trim();
  var p = document.getElementById('wifi-pw').value;
  if (!s) { toast('SSID cannot be empty'); return; }
  if (!confirm('Save credentials and reboot?')) return;
  post('/wifi/update', 'ssid='+encodeURIComponent(s)+'&pw='+encodeURIComponent(p))
    .then(function(r){
      if (r.success) toast('Saved — rebooting...');
      else toast(r.error.message);
    }).catch(function(){ toast('Request failed'); });
}

// Lazy-load pattern list when details opens.
var patternListLoaded = false;
document.querySelector('details').addEventListener('toggle', function(e){
  if (!e.target.open || patternListLoaded) return;
  patternListLoaded = true;
  get('/json/patterns').then(function(r){
    var list = document.getElementById('pattern-list');
    list.innerHTML = '';
    var patterns = r.success ? r.data.patterns : r.patterns;
    patterns.forEach(function(name, i){
      var d = document.createElement('div');
      d.className = 'pi' + (i === state.pattern ? ' active' : '');
      d.textContent = i + '. ' + name;
      d.onclick = function(){
        document.getElementById('pattern-sel').value = i;
        applyPattern();
      };
      list.appendChild(d);
    });
  }).catch(function(){
    document.getElementById('pattern-list').innerHTML =
      '<em style="padding:8px;display:block;color:var(--muted)">Failed to load</em>';
  });
});

// Poll status every 5 s.
refreshStatus();
setInterval(refreshStatus, 5000);
</script>
</body>
</html>
)rawhtml";

// ── Page builder ──────────────────────────────────────────────────────────────

static String buildMainPageHtml() {
  // Build <option> list server-side (avoids a fetch on first load).
  String opts;
  opts.reserve(TOTAL_PATTERNS * 50);
  for (int i = 0; i < TOTAL_PATTERNS; i++) {
    opts += "<option value='";
    opts += i;
    opts += '\'';
    if (i == currentPattern) opts += " selected";
    opts += '>';
    opts += i;
    opts += ". ";
    opts += patternName(i);
    opts += "</option>";
  }

  String page = FPSTR(PAGE_HTML);
  page.replace("%FW_VER%",         FIRMWARE_VERSION);
  page.replace("%PATTERN_OPTIONS%", opts);
  page.replace("%HUE%",            String(gHue));
  page.replace("%SAT%",            String(gSat));
  page.replace("%BRIGHT%",         String(gBrightness));
  page.replace("%SPEED%",          String(gSpeed));
  page.replace("%HOSTNAME%",       DEVICE_HOSTNAME);
  return page;
}

// ── Route setup ───────────────────────────────────────────────────────────────

void setupWebServer() {

  // ── GET / ─────────────────────────────────────────────────────────────────
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(200, "text/html", buildMainPageHtml());
  });

  // ── GET /json/status ──────────────────────────────────────────────────────
  server.on("/json/status", HTTP_GET, [](AsyncWebServerRequest *req) {
    String d = buildStateJson();
    // Inject heap into state JSON (remove trailing } and add field).
    d.remove(d.length() - 1);
    d += ",\"freeHeap\":" + String(ESP.getFreeHeap()) + '}';
    sendOk(req, d);
  });

  // ── GET /json/health ──────────────────────────────────────────────────────
  server.on("/json/health", HTTP_GET, [](AsyncWebServerRequest *req) {
    String d = "{";
    d += "\"uptime\":"       + String(millis() / 1000)                    + ',';
    d += "\"freeHeap\":"     + String(ESP.getFreeHeap())                  + ',';
    d += "\"wifiConnected\":" + String(WiFi.status()==WL_CONNECTED?"true":"false") + ',';
    d += "\"wifiRssi\":"     + String(WiFi.RSSI())                        + ',';
    d += "\"ip\":\""         + WiFi.localIP().toString()                  + "\",";
    d += "\"firmware\":\""   + String(FIRMWARE_VERSION)                   + '"';
    d += '}';
    sendOk(req, d);
  });

  // ── GET /json/patterns ────────────────────────────────────────────────────
  server.on("/json/patterns", HTTP_GET, [](AsyncWebServerRequest *req) {
    String j = "{\"patterns\":[";
    for (int i = 0; i < TOTAL_PATTERNS; i++) {
      if (i) j += ',';
      j += '"';
      j += patternName(i);
      j += '"';
    }
    j += "]}";
    // Wrap in success envelope for consistency; also keep bare array for
    // backward-compat clients that read .patterns directly.
    sendOk(req, j);
  });

  // ── POST /json/pattern ────────────────────────────────────────────────────
  server.on("/json/pattern", HTTP_POST, [](AsyncWebServerRequest *req) {
    int idx;
    if (!getIntParam(req, "index", 0, TOTAL_PATTERNS - 1, idx)) {
      sendErr(req, 400, "INVALID_VALUE",
              "index must be 0–" + String(TOTAL_PATTERNS - 1));
      return;
    }
    currentPattern = idx;
    lastChange = millis();
    markSettingsDirty();
    sendOk(req, buildStateJson());
  });

  // ── POST /json/color (HSV) ────────────────────────────────────────────────
  server.on("/json/color", HTTP_POST, [](AsyncWebServerRequest *req) {
    int h, s, v;
    bool changed = false;
    if (getIntParam(req, "h", 0, 255, h)) { gHue = h; changed = true; }
    if (getIntParam(req, "s", 0, 255, s)) { gSat = s; changed = true; }
    if (getIntParam(req, "v", 0, 255, v)) {
      gBrightness = v;
      FastLED.setBrightness(gBrightness);
      changed = true;
    }
    if (!changed) { sendErr(req, 400, "MISSING_PARAM", "Provide h, s, or v"); return; }
    markSettingsDirty();
    sendOk(req, buildStateJson());
  });

  // ── POST /json/speed ──────────────────────────────────────────────────────
  server.on("/json/speed", HTTP_POST, [](AsyncWebServerRequest *req) {
    int s;
    if (!getIntParam(req, "s", 1, 100, s)) {
      sendErr(req, 400, "INVALID_VALUE", "speed must be 1–100");
      return;
    }
    gSpeed = s;
    markSettingsDirty();
    sendOk(req, buildStateJson());
  });

  // ── POST /json/auto-cycle ─────────────────────────────────────────────────
  server.on("/json/auto-cycle", HTTP_POST, [](AsyncWebServerRequest *req) {
    int s;
    if (!getIntParam(req, "state", 0, 1, s)) {
      sendErr(req, 400, "INVALID_VALUE", "state must be 0 or 1");
      return;
    }
    autoCycle = (s == 1);
    markSettingsDirty();
    sendOk(req, buildStateJson());
  });

  // ── POST /setColor (RGB override) ─────────────────────────────────────────
  server.on("/setColor", HTTP_POST, [](AsyncWebServerRequest *req) {
    int r, g, b;
    if (!getIntParam(req, "r", 0, 255, r) ||
        !getIntParam(req, "g", 0, 255, g) ||
        !getIntParam(req, "b", 0, 255, b)) {
      sendErr(req, 400, "INVALID_VALUE", "r, g, b must be 0–255");
      return;
    }
    setColorPickerColor(r, g, b);
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    sendOk(req, buildStateJson());
  });

  // ── POST /toggleColorMode ─────────────────────────────────────────────────
  server.on("/toggleColorMode", HTTP_POST, [](AsyncWebServerRequest *req) {
    useColorPickerOverride = false;
    sendOk(req, buildStateJson());
  });

  // ── POST /next ────────────────────────────────────────────────────────────
  server.on("/next", HTTP_POST, [](AsyncWebServerRequest *req) {
    currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
    lastChange = millis();
    markSettingsDirty();
    sendOk(req, buildStateJson());
  });

  // ── POST /palette ─────────────────────────────────────────────────────────
  server.on("/palette", HTTP_POST, [](AsyncWebServerRequest *req) {
    int p;
    if (!getIntParam(req, "p", 0, 4, p)) {
      sendErr(req, 400, "INVALID_VALUE", "palette index must be 0–4");
      return;
    }
    switch (p) {
      case 0: currentPalette = RainbowColors_p; break;
      case 1: currentPalette = PartyColors_p;   break;
      case 2: currentPalette = OceanColors_p;   break;
      case 3: currentPalette = HeatColors_p;    break;
      case 4: currentPalette = LavaColors_p;    break;
    }
    sendOk(req, buildStateJson());
  });

  // ── POST /wifi/update ─────────────────────────────────────────────────────
  // Credentials sent in POST body, not URL, to keep them out of server logs.
  server.on("/wifi/update", HTTP_POST, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("ssid", true) || !req->hasParam("pw", true)) {
      sendErr(req, 400, "MISSING_PARAM", "ssid and pw required");
      return;
    }
    String ns = req->getParam("ssid", true)->value();
    String pw = req->getParam("pw",   true)->value();
    ns.trim();
    if (ns.length() == 0 || ns.length() > 31) {
      sendErr(req, 400, "INVALID_VALUE", "SSID must be 1–31 characters");
      return;
    }
    if (pw.length() > 63) {
      sendErr(req, 400, "INVALID_VALUE", "Password must be ≤63 characters");
      return;
    }
    if (!saveWifiToEEPROM(ns, pw)) {
      sendErr(req, 500, "STORAGE_ERROR", "Failed to save credentials");
      return;
    }
    sendOk(req, "{\"message\":\"Saved — rebooting\"}");
    delay(400);
    ESP.restart();
  });

  // ── Palette persistence (LittleFS) ───────────────────────────────────────

  server.on("/api/palettesList", HTTP_GET, [](AsyncWebServerRequest *req) {
    String j = "[";
    bool first = true;
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String fn = dir.fileName();
      if (!fn.startsWith("/palette_")) continue;
      String name = fn.substring(9);
      if (name.endsWith(".txt")) name = name.substring(0, name.length() - 4);
      if (!first) j += ',';
      j += '"'; j += name; j += '"';
      first = false;
    }
    j += ']';
    sendOk(req, j);
  });

  server.on("/api/palette", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("name")) { sendErr(req, 400, "MISSING_PARAM", "name required"); return; }
    String path = "/palette_" + sanitizeName(req->getParam("name")->value()) + ".txt";
    if (!LittleFS.exists(path)) { sendErr(req, 404, "NOT_FOUND", "Palette not found"); return; }
    sendOk(req, csvToJsonArray(readTextFile(path)));
  });

  server.on("/api/savePalette", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("name") || !req->hasParam("colors")) {
      sendErr(req, 400, "MISSING_PARAM", "name and colors required"); return;
    }
    String colors = req->getParam("colors")->value();
    if (colors.length() > 512) { sendErr(req, 400, "INVALID_VALUE", "colors too long"); return; }
    String path = "/palette_" + sanitizeName(req->getParam("name")->value()) + ".txt";
    if (!writeTextFile(path, colors)) { sendErr(req, 500, "STORAGE_ERROR", "Write failed"); return; }
    sendOk(req, "{}");
  });

  server.on("/api/deletePalette", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("name")) { sendErr(req, 400, "MISSING_PARAM", "name required"); return; }
    String path = "/palette_" + sanitizeName(req->getParam("name")->value()) + ".txt";
    if (!LittleFS.exists(path)) { sendErr(req, 404, "NOT_FOUND", "Not found"); return; }
    LittleFS.remove(path);
    sendOk(req, "{}");
  });

  // ── Favorites & color history (LittleFS) ─────────────────────────────────

  server.on("/api/favorites", HTTP_GET, [](AsyncWebServerRequest *req) {
    sendOk(req, csvToJsonArray(readTextFile("/favorites.txt")));
  });

  server.on("/api/saveFavorites", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("fav")) { sendErr(req, 400, "MISSING_PARAM", "fav required"); return; }
    String v = req->getParam("fav")->value();
    if (v.length() > 256) { sendErr(req, 400, "INVALID_VALUE", "too long"); return; }
    if (!writeTextFile("/favorites.txt", v)) { sendErr(req, 500, "STORAGE_ERROR", "Write failed"); return; }
    sendOk(req, "{}");
  });

  server.on("/api/favColors", HTTP_GET, [](AsyncWebServerRequest *req) {
    sendOk(req, csvToJsonArray(readTextFile("/fav_colors.txt")));
  });

  server.on("/api/saveFavColors", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("colors")) { sendErr(req, 400, "MISSING_PARAM", "colors required"); return; }
    String v = req->getParam("colors")->value();
    if (v.length() > 256) { sendErr(req, 400, "INVALID_VALUE", "too long"); return; }
    if (!writeTextFile("/fav_colors.txt", v)) { sendErr(req, 500, "STORAGE_ERROR", "Write failed"); return; }
    sendOk(req, "{}");
  });

  server.on("/api/colorHistory", HTTP_GET, [](AsyncWebServerRequest *req) {
    sendOk(req, csvToJsonArray(readTextFile("/color_history.txt")));
  });

  server.on("/api/saveColorHistory", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (!req->hasParam("colors")) { sendErr(req, 400, "MISSING_PARAM", "colors required"); return; }
    String v = req->getParam("colors")->value();
    if (v.length() > 512) { sendErr(req, 400, "INVALID_VALUE", "too long"); return; }
    if (!writeTextFile("/color_history.txt", v)) { sendErr(req, 500, "STORAGE_ERROR", "Write failed"); return; }
    sendOk(req, "{}");
  });

  // ── 404 catch-all ─────────────────────────────────────────────────────────
  server.onNotFound([](AsyncWebServerRequest *req) {
    sendErr(req, 404, "NOT_FOUND", "Endpoint not found");
  });

  server.begin();
  LOG_INFO("Web server started");
}

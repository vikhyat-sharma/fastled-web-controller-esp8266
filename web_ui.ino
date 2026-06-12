String buildMainPageHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>FastLED Web Controller - Enhanced</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); padding: 8px; margin: 0; min-height: 100vh; }
    h2, h3 { color: #fff; margin: 12px 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
    .container { max-width: 98vw; margin: auto; background: rgba(255,255,255,0.97); padding: 2vw 2vw 2vw 2vw; border-radius: 15px; box-shadow: 0 10px 40px rgba(0,0,0,0.15); }
    .section { border-top: 2px solid #667eea; padding-top: 4vw; margin-top: 4vw; }
    .section:first-child { border-top: none; margin-top: 0; }
    .button, input[type=submit] { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); border: none; color: white; padding: 4vw 6vw; margin: 2vw; border-radius: 8px; font-size: 5vw; font-weight: bold; cursor: pointer; transition: transform 0.2s; min-width: 40vw; }
    .button:hover, input[type=submit]:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4); }
    .button:active, input[type=submit]:active { transform: translateY(0); }
    input[type=range] { width: 100%; max-width: 95vw; margin: 4vw 0; padding: 2vw; font-size: 4vw; border-radius: 6px; border: 2px solid #667eea; cursor: pointer; }
    input[type=range]::-webkit-slider-thumb { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); border-radius: 50%; }
    select { width: 98vw; max-width: 98vw; margin: 4vw 0; padding: 3vw; font-size: 4vw; border-radius: 6px; border: 2px solid #667eea; background-color: white; cursor: pointer; }
    form { margin-bottom: 4vw; }
    details { text-align: left; margin: 4vw auto; max-width: 98vw; border-left: 4px solid #667eea; padding-left: 3vw; }
    summary { cursor: pointer; color: #667eea; font-weight: bold; padding: 2vw; user-select: none; font-size: 4vw; }
    #patternList { margin-top: 4vw; max-height: 60vw; overflow-y: auto; border: 2px solid #ddd; border-radius: 8px; background: #fafafa; padding: 3vw; }
    .pattern-item { padding: 3vw; border-bottom: 1px solid #ececec; font-size: 4vw; cursor: pointer; transition: background 0.2s; border-radius: 4px; }
    .pattern-item:hover { background: #e8eef7; }
    .pattern-item:last-child { border-bottom: none; }
    .color-picker-container { margin: 4vw 0; padding: 4vw; background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%); border-radius: 12px; border: 2px solid #667eea; display: inline-block; color: #333; width: 90vw; }
    #colorPicker { width: 80vw; height: 10vw; margin: 2vw auto; border: 3px solid #667eea; border-radius: 8px; cursor: pointer; display: block; }
    .color-picker-container button { margin: 2vw 2vw; padding: 3vw 6vw; border-radius: 6px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; cursor: pointer; font-weight: bold; font-size: 4vw; }
    .color-picker-container button:hover { transform: scale(1.05); }
    .info-box { background: #e3f2fd; border-left: 4px solid #667eea; padding: 3vw; margin: 3vw 0; border-radius: 6px; text-align: left; font-size: 3vw; }
    .label-text { display: block; margin: 3vw 0 2vw 0; font-weight: bold; color: #333; font-size: 4vw; }
    @media (min-width: 600px) {
      .container { max-width: 800px; padding: 24px; }
      .button, input[type=submit] { font-size: 16px; min-width: 120px; padding: 12px 24px; }
      select, input[type=range] { font-size: 15px; }
      .pattern-item, .label-text, summary { font-size: 16px; }
      .info-box { font-size: 14px; }
      #colorPicker { width: 100px; height: 50px; }
      .color-picker-container { width: auto; }
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>🎨 FastLED Web Controller</h2>
    <h3>Current Pattern: <span style="color: #667eea;">%CURRENT_PATTERN%</span></h3>

    <div class="section">
      <h3>🎨 Color Picker (Works with All Patterns)</h3>
      <div class="color-picker-container">
        <label for="colorPicker" style="font-weight: bold;">Select Color:</label>
        <input type="color" id="colorPicker" value="#0f0000">
        <button onclick="sendColor()">Apply Color</button>
        <button onclick="toggleColorMode()">Disable Color Override</button>
        <button type="button" id="favColorBtn" style="margin-left:2vw;">★ Favorite Color</button>
        <div id="colorFavoritesBar" style="margin:2vw 0;display:flex;flex-wrap:wrap;justify-content:center;"></div>
        <div id="colorHistoryBar" style="margin-top:2vw;display:flex;flex-wrap:wrap;justify-content:center;"></div>
          // Color favorites logic (localStorage)
          const COLOR_FAV_KEY = 'fastled_fav_colors';
          function getFavColors() {
            try { return JSON.parse(localStorage.getItem(COLOR_FAV_KEY)) || []; } catch { return []; }
          }
          function saveFavColors(favs) {
            localStorage.setItem(COLOR_FAV_KEY, JSON.stringify(favs.slice(0, 10)));
          }
          function toggleFavColor(hex) {
            let favs = getFavColors();
            if (favs.includes(hex)) {
              favs = favs.filter(c => c !== hex);
            } else {
              favs.unshift(hex);
            }
            saveFavColors(favs);
            renderFavColors();
          }
          function renderFavColors() {
            const favs = getFavColors();
            const bar = document.getElementById('colorFavoritesBar');
            bar.innerHTML = '';
            if (favs.length === 0) {
              bar.innerHTML = '<span style="color:#888;font-size:3vw;">No favorite colors yet.</span>';
              return;
            }
            favs.forEach(hex => {
              const swatch = document.createElement('div');
              swatch.style.background = hex;
              swatch.style.width = '36px';
              swatch.style.height = '36px';
              swatch.style.margin = '2px';
              swatch.style.border = '3px solid gold';
              swatch.style.borderRadius = '8px';
              swatch.style.cursor = 'pointer';
              swatch.title = hex + ' (favorite)';
              swatch.onclick = () => {
                document.getElementById('colorPicker').value = hex;
              };
              bar.appendChild(swatch);
            });
          }
          window.addEventListener('DOMContentLoaded', () => {
            renderFavColors();
            document.getElementById('favColorBtn').onclick = function() {
              const hex = document.getElementById('colorPicker').value;
              toggleFavColor(hex);
            };
          });
      </div>
      <div class="info-box">
        💡 Select a color and click "Apply Color" to apply it to the current pattern. Use "Disable Color Override" to use pattern defaults.<br>
        Tap a color below to reuse it:
        <span id="colorHistoryHint"></span>
      </div>
    </div>

    <div class="section">
      <h3>Pattern Selection</h3>
      <form action='/pattern' method='GET' style="margin-bottom:2vw;">
        <select id='patternSelect' name='index'>%PATTERN_OPTIONS%</select><br>
        <input type='submit' value='Apply Pattern'>
        <button type="button" id="favPatternBtn" style="margin-left:2vw;">★ Favorite</button>
      </form>
      <div id="favoritePatternsBar" style="margin-bottom:2vw;"></div>

      <details id='allPatterns'>
        <summary>📋 Show all patterns (lazy loaded)</summary>
        <div id='patternList'>Tap to load list...</div>
      </details>
    </div>
    // Pattern favorites logic (localStorage)
    const PATTERN_FAV_KEY = 'fastled_fav_patterns';
    function getFavPatterns() {
      try { return JSON.parse(localStorage.getItem(PATTERN_FAV_KEY)) || []; } catch { return []; }
    }
    function saveFavPatterns(favs) {
      localStorage.setItem(PATTERN_FAV_KEY, JSON.stringify(favs));
    }
    function toggleFavPattern(idx) {
      let favs = getFavPatterns();
      if (favs.includes(idx)) {
        favs = favs.filter(i => i !== idx);
      } else {
        favs.push(idx);
      }
      saveFavPatterns(favs);
      renderFavPatterns();
    }
    function renderFavPatterns() {
      const favs = getFavPatterns();
      const bar = document.getElementById('favoritePatternsBar');
      bar.innerHTML = '';
      if (favs.length === 0) {
        bar.innerHTML = '<span style="color:#888;font-size:3vw;">No favorites yet. Mark patterns as favorites for quick access!</span>';
        return;
      }
      favs.forEach(idx => {
        const btn = document.createElement('button');
        btn.className = 'button';
        btn.style.background = 'linear-gradient(135deg,#ffd700 0%,#ffb300 100%)';
        btn.style.color = '#333';
        btn.style.fontWeight = 'bold';
        btn.style.fontSize = '4vw';
        btn.textContent = '★ ' + patternNames[idx];
        btn.onclick = () => {
          document.getElementById('patternSelect').value = idx;
          document.querySelector('form[action="/pattern"]').submit();
        };
        bar.appendChild(btn);
      });
    }
    // Favorite button logic
    window.addEventListener('DOMContentLoaded', () => {
      renderFavPatterns();
      document.getElementById('favPatternBtn').onclick = function() {
        const idx = parseInt(document.getElementById('patternSelect').value);
        toggleFavPattern(idx);
      };
    });

    <div class="section">
      <h3>🎚️ Color Control (HSV)</h3>
      <form action='/hsv' method='GET'>
        <label class="label-text">Hue (0-255):</label>
        <input type='range' min='0' max='255' name='h' value='%HUE%'><br>
        <label class="label-text">Saturation (0-255):</label>
        <input type='range' min='0' max='255' name='s' value='%SAT%'><br>
        <label class="label-text">Brightness (0-255):</label>
        <input type='range' min='0' max='255' name='v' value='%BRIGHT%'><br>
        <input type='submit' value='Apply HSV'>
      </form>
    </div>

    <div class="section">
      <h3>⚡ Animation Speed</h3>
      <form action='/speed' method='GET'>
        <label class="label-text">Speed (1-100):</label>
        <input type='range' min='1' max='100' name='s' value='%SPEED%'><br>
        <input type='submit' value='Set Speed'>
      </form>
    </div>

    <div class="section">
      <h3>🌈 Color Palette</h3>
      <form action='/palette' method='GET'>
        <select name='p'>
          <option value='0'>🌈 Rainbow</option>
          <option value='1'>🎉 Party</option>
          <option value='2'>🌊 Ocean</option>
          <option value='3'>🔥 Heat</option>
          <option value='4'>🌋 Lava</option>
        </select><br>
        <input type='submit' value='Set Palette'>
      </form>
    </div>

    <div class="section">
      <h3>🔄 Auto Cycle</h3>
      <a href='/autocycle?state=%AUTO_STATE%'><button class='button'>%AUTO_TEXT%</button></a>
    </div>

    <div class="section">
      <h3>🔧 Update WiFi Credentials</h3>
      <form action='/wifi/update' method='GET'>
        <label class="label-text">SSID:</label>
        <input type='text' name='ssid' placeholder='New SSID' style='width:90%;padding:8px;border-radius:6px;border:2px solid #667eea;'><br>
        <label class="label-text">Password:</label>
        <input type='password' name='pw' placeholder='New Password' style='width:90%;padding:8px;border-radius:6px;border:2px solid #667eea;'><br>
        <input type='submit' value='Save & Reboot'>
      </form>
      <div class="info-box">Updating will save credentials to EEPROM and reboot the device.</div>
    </div>
  </div>

  <script>
    let colorOverrideEnabled = true;

    const detailsEl = document.getElementById('allPatterns');
    let loadedPatterns = false;


    // Color history logic
    const COLOR_HISTORY_KEY = 'fastled_color_history';
    function getColorHistory() {
      try {
        return JSON.parse(localStorage.getItem(COLOR_HISTORY_KEY)) || [];
      } catch { return []; }
    }
    function saveColorHistory(hist) {
      localStorage.setItem(COLOR_HISTORY_KEY, JSON.stringify(hist.slice(0, 12)));
    }
    function addColorToHistory(hex) {
      let hist = getColorHistory();
      hist = hist.filter(c => c !== hex);
      hist.unshift(hex);
      saveColorHistory(hist);
      renderColorHistory();
    }
    function renderColorHistory() {
      const hist = getColorHistory();
      const bar = document.getElementById('colorHistoryBar');
      bar.innerHTML = '';
      hist.forEach(hex => {
        const swatch = document.createElement('div');
        swatch.style.background = hex;
        swatch.style.width = '32px';
        swatch.style.height = '32px';
        swatch.style.margin = '2px';
        swatch.style.border = '2px solid #667eea';
        swatch.style.borderRadius = '6px';
        swatch.style.cursor = 'pointer';
        swatch.title = hex;
        swatch.onclick = () => {
          document.getElementById('colorPicker').value = hex;
        };
        bar.appendChild(swatch);
      });
    }

    // --- Advanced Color Picker helpers: harmony, conversions, palettes ---
    function hexToRgb(hex) {
      hex = hex.replace('#','');
      return {
        r: parseInt(hex.substring(0,2),16),
        g: parseInt(hex.substring(2,4),16),
        b: parseInt(hex.substring(4,6),16)
      };
    }

    function rgbToHex(r,g,b){
      return '#' + [r,g,b].map(function(x){
        x = Math.max(0,Math.min(255,Math.round(x)));
        return ('0'+x.toString(16)).slice(-2);
      }).join('');
    }

    function rgbToHsv(r,g,b){
      r/=255; g/=255; b/=255;
      var max = Math.max(r,g,b), min = Math.min(r,g,b);
      var h, s, v = max; var d = max - min;
      s = max === 0 ? 0 : d / max;
      if(max === min){ h = 0; } else {
        switch(max){
          case r: h = (g - b) / d + (g < b ? 6 : 0); break;
          case g: h = (b - r) / d + 2; break;
          case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
      }
      return { h: h*360, s: s*100, v: v*100 };
    }

    function hsvToRgb(h,s,v){
      h = (h%360 + 360) % 360; s/=100; v/=100;
      var c = v * s; var x = c * (1 - Math.abs((h/60) % 2 - 1)); var m = v - c;
      var r1,g1,b1;
      if(h < 60){ r1=c; g1=x; b1=0; }
      else if(h < 120){ r1=x; g1=c; b1=0; }
      else if(h < 180){ r1=0; g1=c; b1=x; }
      else if(h < 240){ r1=0; g1=x; b1=c; }
      else if(h < 300){ r1=x; g1=0; b1=c; }
      else { r1=c; g1=0; b1=x; }
      return { r: Math.round((r1+m)*255), g: Math.round((g1+m)*255), b: Math.round((b1+m)*255) };
    }

    function getComplementaryColor(hex){
      var rgb = hexToRgb(hex);
      var hsv = rgbToHsv(rgb.r, rgb.g, rgb.b);
      hsv.h = (hsv.h + 180) % 360;
      var c = hsvToRgb(hsv.h, hsv.s, hsv.v);
      return rgbToHex(c.r,c.g,c.b);
    }

    function getTriadicColors(hex){
      var rgb = hexToRgb(hex);
      var hsv = rgbToHsv(rgb.r, rgb.g, rgb.b);
      var a = (hsv.h + 120) % 360; var b = (hsv.h + 240) % 360;
      var c1 = hsvToRgb(a,hsv.s,hsv.v); var c2 = hsvToRgb(b,hsv.s,hsv.v);
      return [rgbToHex(c1.r,c1.g,c1.b), rgbToHex(c2.r,c2.g,c2.b)];
    }

    // Palette storage and simple editor (localStorage)
    function savePalette(name, colors){
      if(!name || !colors || !colors.length) return;
      var p = JSON.parse(localStorage.getItem('fastled_palettes')||'{}');
      p[name] = colors.slice(0,16);
      localStorage.setItem('fastled_palettes', JSON.stringify(p));
      renderPalettes();
    }

    function renderPalettes(){
      var container = document.getElementById('paletteList');
      if(!container) return;
      var p = JSON.parse(localStorage.getItem('fastled_palettes')||'{}');
      container.innerHTML = '';
      Object.keys(p).forEach(function(name){
        var row = document.createElement('div'); row.className='palette-row';
        var label = document.createElement('span'); label.textContent = name; row.appendChild(label);
        p[name].forEach(function(hex){
          var sw = document.createElement('span'); sw.className='palette-swatch'; sw.style.background = hex; row.appendChild(sw);
        });
        var use = document.createElement('button'); use.textContent='Use'; use.addEventListener('click', function(){ applyPalette(p[name]); }); row.appendChild(use);
        container.appendChild(row);
      });
    }

    function applyPalette(colors){ if(!colors || !colors.length) return; document.getElementById('colorPicker').value = colors[0]; addColorToHistory(colors[0]); }


    function sendColor() {
      const color = document.getElementById('colorPicker').value;
      const r = parseInt(color.substr(1, 2), 16);
      const g = parseInt(color.substr(3, 2), 16);
      const b = parseInt(color.substr(5, 2), 16);

      fetch(`/setColor?r=${r}&g=${g}&b=${b}`)
        .then(res => res.text())
        .then(text => {
          colorOverrideEnabled = true;
          addColorToHistory(color);
          console.log("Color applied:", text);
          alert("Color applied! " + text);
        })
        .catch(err => alert("Error applying color: " + err));
    }

    // On load, render color history
    window.addEventListener('DOMContentLoaded', renderColorHistory);

    function toggleColorMode() {
      fetch('/toggleColorMode')
        .then(res => res.text())
        .then(text => {
          colorOverrideEnabled = false;
          console.log("Color override disabled:", text);
          alert("Color override disabled. Patterns will use their default colors.");
        })
        .catch(err => alert("Error: " + err));
    }

    detailsEl.addEventListener('toggle', () => {
      if (!detailsEl.open || loadedPatterns) return;

      const list = document.getElementById('patternList');
      list.textContent = 'Loading...';

      fetch('/json/patterns')
        .then((res) => res.json())
        .then((data) => {
          loadedPatterns = true;
          list.innerHTML = '';
          data.patterns.forEach((name, index) => {
            const row = document.createElement('div');
            row.className = 'pattern-item';
            row.textContent = `${index}. ${name}`;
            row.onclick = () => {
              window.location.href = `/pattern?index=${index}`;
            };
            list.appendChild(row);
          });
        })
        .catch(() => {
          list.textContent = 'Failed to load pattern list.';
        });
    });
  </script>
</body>
</html>
)rawliteral";

  html.replace("%CURRENT_PATTERN%", patternNames[currentPattern]);

  String patternOptions;
  patternOptions.reserve(TOTAL_PATTERNS * 45);
  for (int i = 0; i < TOTAL_PATTERNS; i++) {
    patternOptions += "<option value='" + String(i) + "'";
    if (i == currentPattern) patternOptions += " selected";
    patternOptions += ">" + String(i) + ". " + String(patternNames[i]) + "</option>";
  }

  html.replace("%PATTERN_OPTIONS%", patternOptions);
  html.replace("%HUE%", String(gHue));
  html.replace("%SAT%", String(gSat));
  html.replace("%BRIGHT%", String(gBrightness));
  html.replace("%SPEED%", String(gSpeed));
  html.replace("%AUTO_STATE%", String(!autoCycle));
  html.replace("%AUTO_TEXT%", autoCycle ? "Disable AutoCycle" : "Enable AutoCycle");

  return html;
}

void setupWebServer() {
  extern bool saveWifiToEEPROM(const String &newSsid, const String &newPassword);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", buildMainPageHtml());
  });

  server.on("/setColor", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
      uint8_t r = request->getParam("r")->value().toInt();
      uint8_t g = request->getParam("g")->value().toInt();
      uint8_t b = request->getParam("b")->value().toInt();
      
      setColorPickerColor(r, g, b);
      fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
      FastLED.show();

      request->send(200, "text/plain", "Color received and applied!");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/toggleColorMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    useColorPickerOverride = false;
    request->send(200, "text/plain", "Color override disabled");
  });

  server.on("/pattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("index")) {
      int requestedPattern = request->getParam("index")->value().toInt();
      if (requestedPattern >= 0 && requestedPattern < TOTAL_PATTERNS) {
        currentPattern = requestedPattern;
        lastChange = millis();
      }
    }
    request->redirect("/");
  });

  server.on("/hsv", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("h")) gHue = constrain(request->getParam("h")->value().toInt(), 0, 255);
    if (request->hasParam("s")) gSat = constrain(request->getParam("s")->value().toInt(), 0, 255);
    if (request->hasParam("v")) {
      gBrightness = constrain(request->getParam("v")->value().toInt(), 0, 255);
      FastLED.setBrightness(gBrightness);
    }
    request->redirect("/");
  });

  server.on("/autocycle", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("state")) autoCycle = request->getParam("state")->value().toInt();
    request->redirect("/");
  });

  server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request) {
    currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
    request->redirect("/");
  });

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("b")) {
      gBrightness = constrain(request->getParam("b")->value().toInt(), 0, 255);
      FastLED.setBrightness(gBrightness);
    }
    request->redirect("/");
  });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("s")) {
      gSpeed = constrain(request->getParam("s")->value().toInt(), 1, 100);
    }
    request->redirect("/");
  });

  server.on("/json/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"pattern\":" + String(currentPattern) + ",";
    json += "\"hue\":" + String(gHue) + ",";
    json += "\"saturation\":" + String(gSat) + ",";
    json += "\"brightness\":" + String(gBrightness) + ",";
    json += "\"autoCycle\":" + String(autoCycle) + ",";
    json += "\"colorOverride\":" + String(useColorPickerOverride);
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/json/patterns", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"patterns\":[";
    for (int i = 0; i < TOTAL_PATTERNS; i++) {
      json += "\"";
      json += patternNames[i];
      json += "\"";
      if (i < TOTAL_PATTERNS - 1) {
        json += ",";
      }
    }
    json += "]}";
    request->send(200, "application/json", json);
  });

  server.on("/palette", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("p")) {
      int paletteIndex = request->getParam("p")->value().toInt();
      switch (paletteIndex) {
        case 0: currentPalette = RainbowColors_p; break;
        case 1: currentPalette = PartyColors_p; break;
        case 2: currentPalette = OceanColors_p; break;
        case 3: currentPalette = HeatColors_p; break;
        case 4: currentPalette = LavaColors_p; break;
      }
    }
    request->redirect("/");
  });

  server.on("/wifi/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid") && request->hasParam("pw")) {
      String ns = request->getParam("ssid")->value();
      String pw = request->getParam("pw")->value();
      if (ns.length() == 0) {
        request->send(400, "text/plain", "SSID cannot be empty");
        return;
      }
      bool ok = saveWifiToEEPROM(ns, pw);
      if (ok) {
        request->send(200, "text/plain", "Saved credentials, rebooting...");
        delay(500);
        ESP.restart();
      } else {
        request->send(500, "text/plain", "Failed to save credentials");
      }
    } else {
      request->send(400, "text/plain", "Missing ssid or pw");
    }
  });

  server.begin();
}

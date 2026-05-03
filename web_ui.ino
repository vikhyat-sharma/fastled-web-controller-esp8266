String buildMainPageHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>FastLED Web Controller - Enhanced</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); padding: 16px; margin: 0; min-height: 100vh; }
    h2, h3 { color: #fff; margin: 12px 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
    .container { max-width: 800px; margin: auto; background: rgba(255,255,255,0.95); padding: 24px; border-radius: 15px; box-shadow: 0 10px 40px rgba(0,0,0,0.2); }
    .section { border-top: 2px solid #667eea; padding-top: 16px; margin-top: 16px; }
    .section:first-child { border-top: none; margin-top: 0; }
    .button { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); border: none; color: white; padding: 12px 24px; margin: 8px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; transition: transform 0.2s; }
    .button:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4); }
    .button:active { transform: translateY(0); }
    input[type=range] { width: 100%; max-width: 500px; margin: 12px 0; padding: 8px; font-size: 15px; border-radius: 6px; border: 2px solid #667eea; cursor: pointer; }
    input[type=range]::-webkit-slider-thumb { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); border-radius: 50%; }
    select { width: 95%; max-width: 500px; margin: 12px 0; padding: 10px; font-size: 15px; border-radius: 6px; border: 2px solid #667eea; background-color: white; cursor: pointer; }
    input[type=submit] { padding: 10px 20px; font-size: 15px; border-radius: 6px; border: 2px solid #667eea; background-color: #f0f2f5; cursor: pointer; transition: all 0.2s; }
    input[type=submit]:hover { background-color: #667eea; color: white; }
    form { margin-bottom: 20px; }
    details { text-align: left; margin: 16px auto; max-width: 650px; border-left: 4px solid #667eea; padding-left: 12px; }
    summary { cursor: pointer; color: #667eea; font-weight: bold; padding: 8px; user-select: none; }
    #patternList { margin-top: 12px; max-height: 350px; overflow-y: auto; border: 2px solid #ddd; border-radius: 8px; background: #fafafa; padding: 12px; }
    .pattern-item { padding: 10px; border-bottom: 1px solid #ececec; font-size: 14px; cursor: pointer; transition: background 0.2s; border-radius: 4px; }
    .pattern-item:hover { background: #e8eef7; }
    .pattern-item:last-child { border-bottom: none; }
    .color-picker-container { margin: 16px 0; padding: 20px; background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%); border-radius: 12px; border: 2px solid #667eea; display: inline-block; color: #333; }
    #colorPicker { width: 100px; height: 50px; margin: 10px auto; border: 3px solid #667eea; border-radius: 8px; cursor: pointer; display: block; }
    .color-picker-container button { margin: 10px 5px; padding: 10px 20px; border-radius: 6px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; cursor: pointer; font-weight: bold; }
    .color-picker-container button:hover { transform: scale(1.05); }
    .info-box { background: #e3f2fd; border-left: 4px solid #667eea; padding: 12px; margin: 12px 0; border-radius: 6px; text-align: left; font-size: 14px; }
    .label-text { display: block; margin: 10px 0 5px 0; font-weight: bold; color: #333; }
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
        <input type="color" id="colorPicker" value="#ff0000">
        <button onclick="sendColor()">Apply Color</button>
        <button onclick="toggleColorMode()">Disable Color Override</button>
      </div>
      <div class="info-box">
        💡 Select a color and click "Apply Color" to apply it to the current pattern. Use "Disable Color Override" to use pattern defaults.
      </div>
    </div>

    <div class="section">
      <h3>Pattern Selection</h3>
      <form action='/pattern' method='GET'>
        <select id='patternSelect' name='index'>%PATTERN_OPTIONS%</select><br>
        <input type='submit' value='Apply Pattern'>
      </form>

      <details id='allPatterns'>
        <summary>📋 Show all patterns (lazy loaded)</summary>
        <div id='patternList'>Tap to load list...</div>
      </details>
    </div>

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
  </div>

  <script>
    let colorOverrideEnabled = true;

    const detailsEl = document.getElementById('allPatterns');
    let loadedPatterns = false;

    function sendColor() {
      const color = document.getElementById('colorPicker').value;
      const r = parseInt(color.substr(1, 2), 16);
      const g = parseInt(color.substr(3, 2), 16);
      const b = parseInt(color.substr(5, 2), 16);

      fetch(`/setColor?r=${r}&g=${g}&b=${b}`)
        .then(res => res.text())
        .then(text => {
          colorOverrideEnabled = true;
          console.log("Color applied:", text);
          alert("Color applied! " + text);
        })
        .catch(err => alert("Error applying color: " + err));
    }

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
    if (request->hasParam("h")) gHue = request->getParam("h")->value().toInt();
    if (request->hasParam("s")) gSat = request->getParam("s")->value().toInt();
    if (request->hasParam("v")) {
      gBrightness = request->getParam("v")->value().toInt();
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
      gBrightness = request->getParam("b")->value().toInt();
      FastLED.setBrightness(gBrightness);
    }
    request->redirect("/");
  });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("s")) {
      gSpeed = request->getParam("s")->value().toInt();
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

  server.begin();
}

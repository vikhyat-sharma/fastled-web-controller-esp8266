String buildMainPageHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>ESP8266 FastLED Controller</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f2f5; padding: 16px; margin: 0; }
    h2, h3 { color: #333; margin: 12px 0; }
    .container { max-width: 700px; margin: auto; background: #fff; padding: 18px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    .section { border-top: 1px solid #eee; padding-top: 12px; margin-top: 12px; }
    .button { background-color: #4CAF50; border: none; color: white; padding: 10px 16px; margin: 5px; border-radius: 8px; font-size: 16px; cursor: pointer; }
    .button:hover { background-color: #45a049; }
    input[type=range], select { width: 85%; max-width: 420px; margin: 8px 0; padding: 8px; font-size: 15px; border-radius: 6px; border: 1px solid #ccc; }
    input[type=submit] { padding: 8px 14px; font-size: 15px; border-radius: 6px; border: 1px solid #ccc; }
    form { margin-bottom: 20px; }
    details { text-align: left; margin: 12px auto; max-width: 580px; }
    summary { cursor: pointer; color: #1e5fbf; font-weight: bold; }
    #patternList { margin-top: 8px; max-height: 260px; overflow-y: auto; border: 1px solid #ddd; border-radius: 6px; background: #fafafa; padding: 8px; }
    .pattern-item { padding: 6px 4px; border-bottom: 1px solid #ececec; font-size: 14px; }
    .pattern-item:last-child { border-bottom: none; }
    .color-picker-container { margin: 10px 0; padding: 10px; background: #1c1c1c; border-radius: 10px; display: inline-block; color: white; }
    #colorPicker { width: 60px; height: 35px; margin-left: 10px; vertical-align: middle; }
    .color-picker-container button { margin-left: 10px; padding: 6px 12px; border-radius: 5px; background-color: #444; color: white; border: none; cursor: pointer; }
    .color-picker-container button:hover { background-color: #666; }
  </style>
</head>
<body>
  <div class="container">
    <h2>ESP8266 FastLED Controller</h2>
    <h3>Current Pattern: %CURRENT_PATTERN%</h3>

    <div class="section">
      <h3>Pattern Selection</h3>
      <form action='/pattern' method='GET'>
        <select id='patternSelect' name='index'>%PATTERN_OPTIONS%</select><br>
        <input type='submit' value='Apply Pattern'>
      </form>

      <details id='allPatterns'>
        <summary>Show all patterns (lazy loaded)</summary>
        <div id='patternList'>Tap to load list...</div>
      </details>
    </div>

    <div class="section">
      <h3>Color Control</h3>
      <form action='/hsv' method='GET'>
        Hue: <input type='range' min='0' max='255' name='h' value='%HUE%'><br>
        Saturation: <input type='range' min='0' max='255' name='s' value='%SAT%'><br>
        Brightness: <input type='range' min='0' max='255' name='v' value='%BRIGHT%'><br>
        <input type='submit' value='Set'>
      </form>
    </div>

    <div class="section">
      <h3>Animation Speed</h3>
      <form action='/speed' method='GET'>
        Speed (1-100): <input type='range' min='1' max='100' name='s' value='%SPEED%'><br>
        <input type='submit' value='Set'>
      </form>
    </div>

    <div class="section">
      <h3>Color Palette</h3>
      <form action='/palette' method='GET'>
        <select name='p'>
          <option value='0'>Rainbow</option>
          <option value='1'>Party</option>
          <option value='2'>Ocean</option>
          <option value='3'>Heat</option>
          <option value='4'>Lava</option>
        </select><br>
        <input type='submit' value='Set'>
      </form>
    </div>

    <div class="section">
      <h3>Auto Cycle</h3>
      <a href='/autocycle?state=%AUTO_STATE%'><button class='button'>%AUTO_TEXT%</button></a>
    </div>

    <div class="section">
      <h3>Color Picker</h3>
      <div class="color-picker-container">
        <label for="colorPicker">Select Color:</label>
        <input type="color" id="colorPicker" value="#ff0000">
        <button onclick="sendColor()">Set Color</button>
      </div>
    </div>
  </div>

  <script>
    const detailsEl = document.getElementById('allPatterns');
    let loadedPatterns = false;

    function sendColor() {
      const color = document.getElementById('colorPicker').value;
      const r = parseInt(color.substr(1, 2), 16);
      const g = parseInt(color.substr(3, 2), 16);
      const b = parseInt(color.substr(5, 2), 16);

      fetch(`/setColor?r=${r}&g=${g}&b=${b}`)
        .then(res => res.text())
        .then(text => console.log("Color set:", text));
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
  html.replace("%AUTO_TEXT%", autoCycle ? "Disable" : "Enable");

  return html;
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", buildMainPageHtml());
  });

  server.on("/setColor", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
      customColorR = request->getParam("r")->value().toInt();
      customColorG = request->getParam("g")->value().toInt();
      customColorB = request->getParam("b")->value().toInt();

      Serial.printf("Custom color set: R=%d, G=%d, B=%d\n", customColorR, customColorG, customColorB);
      fill_solid(leds, NUM_LEDS, CRGB(customColorR, customColorG, customColorB));

      request->send(200, "text/plain", "Color received");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/pattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("index")) {
      int requestedPattern = request->getParam("index")->value().toInt();
      if (requestedPattern >= 0 && requestedPattern < TOTAL_PATTERNS) {
        currentPattern = requestedPattern;
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
    json += "\"autoCycle\":" + String(autoCycle);
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

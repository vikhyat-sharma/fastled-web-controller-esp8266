# FastLED Web Controller (ESP8266)

Web-based LED strip controller built with ESP8266, FastLED, and AsyncWebServer.

## Features

- Web UI for pattern selection and live controls
- Curated high-quality pattern set (less duplicates, more distinct visuals)
- Real-time control for:
  - Brightness
  - Speed
  - Hue
  - Saturation
- Auto-cycle mode
- JSON endpoints for status and pattern listing
- Modular Arduino tabs for maintainability

## Project Structure

- `fastLED.ino` - core setup, state, and pattern implementations
- `web_ui.ino` - HTML UI + API routes
- `pattern_runner.ino` - pattern dispatch switch
- `secrets.h` - local Wi-Fi credentials (ignored by git)
- `secrets.example.h` - template credentials file

## Setup

1. Install required libraries in Arduino IDE:
   - FastLED
   - ESPAsyncWebServer
   - ESP8266WiFi
2. Copy `secrets.example.h` to `secrets.h`.
3. Update `ssid` and `password` in `secrets.h`.
4. Select your ESP8266 board and upload.
5. Open Serial Monitor to get the assigned IP.
6. Open that IP in your browser.

## API Endpoints

- `GET /` - main UI
- `GET /json/status` - current state JSON
- `GET /json/patterns` - available pattern names

## Notes

- `secrets.h` is intentionally excluded from git to keep credentials private.
- Speed now scales frame delay consistently for curated patterns.

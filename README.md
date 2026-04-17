# FastLED Web Controller (ESP8266)

A lightweight, web-based LED strip controller powered by ESP8266, FastLED, and AsyncWebServer. Control your LED patterns in real time through a responsive browser interface.

## Features

* **Interactive Web UI** for selecting and controlling LED patterns
* **Curated Pattern Set** with distinct, high-quality visual effects (no clutter or duplicates)
* **Live Controls** for:

  * Brightness
  * Speed
  * Hue
  * Saturation
* **Auto-Cycle Mode** to rotate through patterns automatically
* **JSON API Endpoints** for integration and external control
* **Modular Codebase** using Arduino tabs for better organization and maintainability

## Project Structure

* `fastLED.ino` — Core setup, global state, and pattern implementations
* `web_ui.ino` — Web interface and API route handling
* `pattern_runner.ino` — Pattern dispatcher logic
* `secrets.h` — Wi-Fi credentials (excluded from version control)
* `secrets.example.h` — Template for credentials setup

## Getting Started

1. Install the required libraries in the Arduino IDE:

   * FastLED
   * ESPAsyncWebServer
   * ESP8266WiFi

2. Create your credentials file:

   * Copy `secrets.example.h` → `secrets.h`
   * Add your Wi-Fi `ssid` and `password`

3. Upload the code:

   * Select your ESP8266 board
   * Compile and upload the project

4. Access the controller:

   * Open the Serial Monitor to find the assigned IP address
   * Enter the IP in your browser

## API Endpoints

* `GET /` — Web interface
* `GET /json/status` — Current system state (JSON)
* `GET /json/patterns` — List of available patterns

## Notes

* `secrets.h` is excluded from version control to protect sensitive information
* Speed control is normalized to ensure consistent animation timing across all curated patterns

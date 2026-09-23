# FastLED Web Controller (ESP8266)

![Platform](https://img.shields.io/badge/platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/framework-Arduino-lightgrey)
![License](https://img.shields.io/badge/license-MIT-green)

A lightweight, web-based LED strip controller built on ESP8266, FastLED, and AsyncWebServer. It provides a responsive browser interface for real-time control of LED animations with a focus on reliability, maintainability, and a polished UX.

---

## ✨ Features

* 🌐 **Responsive Web Interface** for pattern selection and live control
* 🎨 **Curated Effects Library** — no redundant or low-quality animations
* ⚡ **Real-Time Controls**

  * Brightness
  * Speed
  * Hue
  * Saturation
* 🔁 **Auto-Cycle Mode** for hands-free animation playback
* 🔌 **JSON API** for external integrations and automation
* 🧩 **Modular Architecture** for easy extension and maintenance

---

## 📦 Requirements

### Hardware

* ESP8266 development board (NodeMCU, Wemos D1 Mini, or similar)
* Addressable LED strip (WS2812B / NeoPixel compatible)
* 5V power supply sized for your strip length
* USB cable for flashing

### Software

* Arduino IDE or Arduino CLI
* ESP8266 board support installed

### Libraries

Install via the Arduino Library Manager:

* FastLED
* ESPAsyncWebServer
* ESP8266WiFi
* EEPROM
* LittleFS

> Note: `ESPAsyncWebServer` may also require `ESPAsyncTCP` depending on your environment.

### Network

* 2.4 GHz Wi-Fi network (ESP8266 does not support 5 GHz)

---

## 🔌 Wiring Diagram

### Basic Connections (ESP8266 + WS2812B)

ESP8266 (NodeMCU / Wemos D1 Mini) WS2812B LED Strip

3V3 / 5V (external PSU recommended) -> VCC (+5V)
GND ---------------------------------> GND
D4 (GPIO2) --------------------------> DIN


### 🖼️ Visual Wiring Diagram

                +5V Power Supply
                     │
                     │
          ┌──────────┴──────────┐
          │                     │
    ┌─────▼─────┐         ┌─────▼──────────────┐
    │  LED Strip │         │    ESP8266         │
    │  WS2812B   │         │ (NodeMCU/Wemos)    │
    │             │         │                    │
    │   VCC ◄─────┼─────────┼──── 5V             │
    │   GND ◄─────┼─────────┼──── GND            │
    │   DIN ◄─────┼──[330Ω]─┼──── D4 (GPIO2)    │
    └─────────────┘         └────────────────────┘
             │
             │
    ┌────────▼────────┐
    │ 1000µF Capacitor│
    │ (Across VCC/GND)│
    └─────────────────┘


### ⚠️ Important Notes

- Common ground is required between ESP8266 and LED strip  
- Add a 330–470Ω resistor on the data line  
- Add a 1000µF capacitor across LED power terminals  
- Use external 5V power for larger LED strips  
- Consider a logic level shifter for long runs  

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/vikhyat-sharma/fastled-web-controller.git
cd fastled-web-controller
```

### 2. Create your Wi-Fi config

Copy the example file and update the credentials:

```bash
cp secrets.example.h secrets.h
```

Then edit the file and set your Wi-Fi SSID and password.

### 3. Flash the firmware

Open the sketch in Arduino IDE or use Arduino CLI, then compile and upload to your ESP8266 board.

### 4. Connect and control

After boot, the board will connect to Wi-Fi and expose the web interface. Open the device IP address or `http://fastled.local` if mDNS is available.

---

## 🗂️ Project Structure

```
fastLED.ino          # Boot sequence, Wi-Fi startup, and runtime loop
web_ui.ino           # Web UI markup, API routes, and persistence helpers
pattern_runner.ino   # Pattern dispatch and registration
constants.h          # Configuration values and pattern catalog
colormanagement.h    # Color state helpers and frame timing utilities
patterns.h           # Animation implementations
```

---

## 🔌 API Reference

| Endpoint | Method | Description |
| --- | --- | --- |
| `/` | GET | Web interface |
| `/json/status` | GET | Current controller state and memory usage |
| `/json/patterns` | GET | List of available patterns |
| `/setColor` | GET | Apply a direct RGB color override |
| `/hsv` | GET | Adjust hue, saturation, and brightness |
| `/pattern` | GET | Switch the active pattern |
| `/api/palettesList` | GET | List saved palettes |
| `/api/savePalette` | GET | Save a palette to LittleFS |

---

## 🔧 Configuration

Common parameters you may want to tweak:

* LED pin and count
* Default brightness
* Pattern transition timing
* Auto-cycle interval

---

## 🖼️ Preview

> *TODO(Add screenshots or GIFs here)*

---

## 🛠️ Roadmap

* [ ] OTA firmware updates
* [ ] Preset and scene saving
* [ ] Mobile-specific UI refinements
* [ ] MQTT / Home Assistant integration
* [ ] Additional pattern categories and presets

---

## 🤝 Contributing

Contributions are welcome. If you’d like to improve patterns, UI, or performance:

1. Fork the repository
2. Create a feature branch
3. Submit a pull request

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for repository conventions and PR expectations.

---

## 🔒 Security

* `secrets.h` is excluded from version control to protect credentials
* Do not expose your device directly to the public internet without proper security

---

## 📄 License

This project is licensed under the MIT License. See `LICENSE` for details.

---
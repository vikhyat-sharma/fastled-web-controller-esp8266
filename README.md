# FastLED Web Controller (ESP8266)

![Platform](https://img.shields.io/badge/platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/framework-Arduino-lightgrey)
![License](https://img.shields.io/badge/license-MIT-green)

A lightweight, web-based LED strip controller built on ESP8266, FastLED, and AsyncWebServer. It provides a responsive browser interface for real-time control of LED animations, designed with performance, simplicity, and clean visuals in mind.

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

* ESP8266 development board (e.g., NodeMCU, Wemos D1 Mini)
* Addressable LED strip (WS2812B / NeoPixel or compatible)
* Adequate power supply (based on LED count)
* USB cable for flashing

### Software

* Arduino IDE (latest recommended)
* ESP8266 board support installed

### Libraries

Install via Arduino Library Manager:

* FastLED
* ESPAsyncWebServer
* ESP8266WiFi

> ℹ️ `ESPAsyncWebServer` may also require `ESPAsyncTCP` depending on your setup.

### Network

* 2.4 GHz Wi-Fi network (ESP8266 does not support 5 GHz)

---

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/vikhyat-sharma/fastled-web-controller.git
cd fastled-web-controller
```

### 2. Configure Wi-Fi Credentials

```cpp
// secrets.h
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```

* Copy `secrets.example.h` → `secrets.h`
* Update credentials

### 3. Upload to ESP8266

* Select your board in Arduino IDE
* Compile and upload

### 4. Access the Web Interface

* Open Serial Monitor (baud rate as configured)
* Note the device IP address
* Open it in your browser

---

## 🗂️ Project Structure

```
fastLED.ino          # Core setup, state, and patterns
web_ui.ino           # Web UI + API routes
pattern_runner.ino   # Pattern dispatch logic
secrets.h            # Local credentials (ignored by git)
secrets.example.h    # Template credentials file
```

---

## 🔌 API Reference

| Endpoint         | Method | Description                |
| ---------------- | ------ | -------------------------- |
| `/`              | GET    | Web interface              |
| `/json/status`   | GET    | Current controller state   |
| `/json/patterns` | GET    | List of available patterns |

---

## 🔧 Configuration

Common parameters you may want to tweak:

* LED pin and count
* Default brightness
* Pattern transition timing
* Auto-cycle interval

---

## 🖼️ Preview

> *(Add screenshots or GIFs here for better engagement)*

---

## 🛠️ Roadmap

* [ ] Save presets
* [ ] OTA firmware updates
* [ ] Mobile UI improvements
* [ ] MQTT / Home Assistant integration

---

## 🤝 Contributing

Contributions are welcome. If you’d like to improve patterns, UI, or performance:

1. Fork the repository
2. Create a feature branch
3. Submit a pull request

---

## 🔒 Security

* `secrets.h` is excluded from version control to protect credentials
* Do not expose your device directly to the public internet without proper security

---

## 📄 License

This project is licensed under the MIT License. See `LICENSE` for details.

---

If you want, I can next:

* Add a **wiring diagram section (with diagram)**
* Generate a **GIF demo workflow**
* Or tailor this specifically for **Home Assistant / IoT enthusiasts**, which gets a lot more traction on GitHub

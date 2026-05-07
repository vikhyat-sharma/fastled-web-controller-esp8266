# FastLED Web Controller - ESP8266 Project

A sophisticated web-controlled LED animation system for the ESP8266 with 61+ stunning patterns, color picker integration, and modular architecture.

## 🎯 Features

### ✨ Pattern Library (61 Patterns)
- **Rainbow & Color Cycling** (9 patterns): Smooth color transitions and gradients
- **Movement & Chase** (8 patterns): Dynamic moving lights and chasing effects
- **Dynamic & Interactive** (9 patterns): Confetti, sparkles, and random effects
- **Breathing & Pulse** (6 patterns): Rhythmic pulsing and breathing effects
- **Wave & Ripple** (4 patterns): Wave formations and ripple effects
- **Fire & Heat** (4 patterns): Realistic fire and lava simulations
- **Electric & Neon** (3 patterns): High-energy electric and neon effects
- **Nature & Weather** (5 patterns): Aurora, ocean, storm effects
- **Cosmic & Space** (6 patterns): Galaxy and plasma effects
- **Special & Festive** (7 patterns): Holiday and special occasion themes

### 🎨 Advanced Features
- **Universal Color Picker**: Apply any color to patterns
- **HSV Color Control**: Fine-tune Hue, Saturation, and Brightness
- **Speed Control**: Adjust animation speed from 1-100
- **Color Palettes**: Switch between 5 predefined palettes
- **Auto-Cycle Mode**: Automatically cycle through patterns
- **Responsive Web UI**: Beautiful gradient interface optimized for mobile

### 🏗️ Modular Architecture
- **patterns.h**: 61 organized pattern functions by category
- **colormanagement.h**: Centralized color and speed management
- **constants.h**: Configuration and pattern registry
- **web_ui.ino**: Web server and HTTP endpoints
- **pattern_runner.ino**: Pattern execution dispatcher
- **fastLED.ino**: Main setup and loop functions

---

## 📁 Project Structure

```
fastled-web-controller-esp8266/
├── fastLED.ino                 # Main file with setup/loop
├── web_ui.ino                  # Web server and UI HTML
├── pattern_runner.ino          # Pattern dispatcher switch statement
├── patterns.h                  # 61 pattern functions (NEW)
├── colormanagement.h           # Color/speed/brightness management (NEW)
├── constants.h                 # Pin/LED config and pattern names
├── secrets.h                   # WiFi credentials (gitignored)
├── secrets.example.h           # Example secrets file
├── README.md                   # This file
└── FUTURE_ENHANCEMENTS.md      # Roadmap for future features
```

---

## 🚀 Getting Started

### Hardware Requirements
- ESP8266 (D1 Mini, NodeMCU, etc.)
- WS2812B addressable LED strip (NeoPixel compatible)
- USB power supply
- Jumper wires

### Hardware Connections
```
ESP8266    →    WS2812B LED Strip
GND        →    GND
GPIO4 (D2) →    Data Pin (DIN)
5V Power   →    5V Power
```

### Installation

1. **Clone/Download Project**
   ```bash
   git clone <repo-url>
   cd fastled-web-controller-esp8266
   ```

2. **Set Up Arduino IDE**
   - Install ESP8266 board: Sketch → Include Library → Manage Libraries
   - Search for "ESP8266" and install
   - Select Board: Tools → Board → ESP8266 Boards → LOLIN(WEMOS) D1 R32 (or your board)

3. **Install Required Libraries**
   - FastLED: Sketch → Include Library → Manage Libraries → Search "FastLED"
   - ESPAsyncWebServer: Sketch → Include Library → Manage Libraries → Search "ESPAsyncWebServer"

4. **Configure WiFi**
   ```bash
   # Copy example secrets
   cp secrets.example.h secrets.h
   
   # Edit secrets.h with your WiFi credentials
   const char *ssid = "YOUR_SSID";
   const char *password = "YOUR_PASSWORD";
   ```

5. **Adjust LED Configuration in constants.h**
   ```cpp
   #define LED_PIN D2              // GPIO pin for data
   #define NUM_LEDS 168            // Number of LEDs
   #define BRIGHTNESS 128          // Default brightness
   #define LED_TYPE WS2812B        // LED type
   #define COLOR_ORDER GRB         // Color order (RGB, GRB, BGR, etc.)
   ```

6. **Upload to ESP8266**
   - Sketch → Upload
   - Wait for upload to complete

7. **Connect to Controller**
   - Open Serial Monitor: Tools → Serial Monitor (115200 baud)
   - Find the device IP address in the output
   - Open browser: `http://<IP_ADDRESS>` or `http://fastled.local`

---

## 🎮 Web Interface Guide

### Main Controls

**🎨 Color Picker (Works with All Patterns)**
- Select any color using the HTML5 color picker
- Click "Apply Color" to instantly apply to current pattern
- Click "Disable Color Override" to return to pattern defaults

**Pattern Selection**
- Use dropdown menu to select from 61 patterns
- Or expand "Show all patterns" to browse and click directly
- Current pattern displays at the top

**🎚️ Color Control (HSV)**
- **Hue** (0-255): Choose the base color (0=red, 85=green, 170=blue)
- **Saturation** (0-255): Intensity of color (0=grayscale, 255=full saturation)
- **Brightness** (0-255): Overall LED brightness (0=off, 255=max)

**⚡ Animation Speed**
- Slider from 1-100
- Lower values = slower animations
- Higher values = faster animations
- Affects the delay() in pattern functions

**🌈 Color Palette**
- Rainbow: Full spectrum
- Party: Vibrant mixed colors
- Ocean: Blue/cyan/teal shades
- Heat: Red/orange/yellow (fire simulation)
- Lava: Orange/red/yellow gradients

**🔄 Auto Cycle**
- Enable: Automatically cycle through all 61 patterns every ~13 seconds
- Disable: Stop on current pattern
- Toggle with button

---

## 🔧 Adding New Patterns

### Pattern Template

```cpp
// ============================================================
// Pattern: Your Pattern Name
// Category: [Rainbow/Movement/Dynamic/Breathing/Wave/Fire/Electric/Nature/Cosmic/Special]
// Description: Brief description of what the pattern does
// ============================================================

void yourPatternName() {
  static uint8_t someState = 0;  // Persistent state between frames
  
  // Fade existing LEDs
  fadeToBlackBy(leds, NUM_LEDS, 20);
  
  // Draw your pattern
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + i * 5, gSat, 200);
  }
  
  // Update animation
  someState++;
  
  // Show frame with base delay (will be scaled by speed control)
  showFrame(20);  // 20ms base delay
}
```

### Steps to Add a Pattern

1. **Add function to patterns.h**
   - Follow the template above
   - Add helpful comments
   - Use `showFrame()` instead of `FastLED.show()`

2. **Update constants.h**
   ```cpp
   const char *patternNames[] = {
     // ... existing patterns ...
     "Your Pattern Name"  // Add your pattern name
   };
   ```

3. **Update pattern_runner.ino**
   ```cpp
   void runCurrentPattern() {
     switch (currentPattern) {
       // ... existing cases ...
       case 61: yourPatternName(); break;  // Add your case
       // ...
     }
   }
   ```

4. **Test & Validate**
   - Compile and upload
   - Verify pattern displays correctly
   - Check for memory issues
   - Ensure smooth animation

---

## 💡 Global Variables Reference

### Color Variables (colormanagement.h)
```cpp
uint8_t gHue = 0;               // Global hue (0-255)
uint8_t gSat = 255;             // Global saturation (0-255)
uint8_t gBrightness = 128;      // Global brightness (0-255)
uint8_t gSpeed = 20;            // Animation speed (1-100)
uint8_t colorPickerR/G/B;       // Current color picker RGB values
bool useColorPickerOverride;    // Whether to apply color picker
CRGBPalette16 currentPalette;   // Current color palette
```

### State Variables (fastLED.ino)
```cpp
uint8_t currentPattern = 0;     // Current pattern index
bool autoCycle = true;          // Auto-cycle enabled
unsigned long lastChange = 0;   // Last pattern change time
```

### LED Array
```cpp
CRGB leds[NUM_LEDS];  // Main LED array
```

---

## 🎨 Color Space Guide

### HSV (Hue, Saturation, Value)
- **Hue** (0-255): Position on color wheel
  - 0° = Red, 85° = Green, 170° = Blue, 255° = Red again
- **Saturation** (0-255): Color intensity
  - 0 = Grayscale, 255 = Full color
- **Value/Brightness** (0-255): Overall brightness
  - 0 = Off, 255 = Maximum brightness

### RGB (Red, Green, Blue)
- Direct color mixing (0-255 per channel)
- Used in color picker input
- Converted to HSV for pattern use

### Common Color Values (HSV)
```
Red:        CHSV(0,   255, 255)
Orange:     CHSV(32,  255, 255)
Yellow:     CHSV(64,  255, 255)
Green:      CHSV(96,  255, 255)
Cyan:       CHSV(128, 255, 255)
Blue:       CHSV(160, 255, 255)
Magenta:    CHSV(224, 255, 255)
White:      CHSV(0,   0,   255)
Black:      CHSV(0,   0,   0)
```

---

## ⚡ Performance Tips

1. **Memory Usage**
   - LED array: 3 bytes per LED (RGB)
   - 168 LEDs = 504 bytes minimum
   - Keep patterns efficient to avoid heap fragmentation

2. **Frame Rate**
   - Target 60+ FPS for smooth animation
   - Use `showFrame(baseDelay)` to maintain timing
   - Faster patterns (lower delay) = lower perceived frame time

3. **Power Consumption**
   - Full brightness white = ~60mA per LED
   - 168 LEDs at full white = ~10 amps!
   - Use `FastLED.setBrightness()` to limit power
   - Limit to ~5A for safety (about 80 LEDs at full brightness)

4. **WiFi Communication**
   - WiFi doesn't interfere with LED timing (different GPIO)
   - But Web requests can cause brief delays
   - Use AsyncWebServer for non-blocking requests

---

## 🐛 Troubleshooting

### LEDs don't light up
- Check GPIO pin matches `LED_PIN` in constants.h
- Verify power supply is connected and adequate
- Check data line connection (should be D2/GPIO4)
- Verify LED_TYPE matches your strip

### WiFi won't connect
- Check SSID and password in secrets.h
- Verify WiFi credentials are correct
- Check 2.4GHz (ESP8266 doesn't support 5GHz)
- Check WiFi range

### Web interface is slow
- Ensure good WiFi signal
- Check if other devices are using bandwidth
- Reduce number of clients connected
- Try accessing via IP instead of mDNS

### Patterns are jerky/laggy
- Reduce animation speed
- Check if other tasks are running
- Verify power supply is adequate
- Check for memory leaks

### Memory errors
- Reduce `NUM_LEDS` if too many LEDs
- Check for static arrays in patterns
- Use PROGMEM for large constant arrays

---

## 📊 Technical Specifications

| Property | Value |
|----------|-------|
| Microcontroller | ESP8266 |
| WiFi | 802.11 b/g/n (2.4 GHz) |
| RAM | 160 KB |
| Flash | 4 MB |
| LED Protocol | WS2812B (NeoPixel) |
| Max Patterns | 255 (currently 61) |
| Update Frequency | ~50 FPS typical |
| Power Consumption | <200mA (ESP) + LED power |

---

## 🔐 Security Notes

- Change WiFi password regularly
- Consider adding HTTP authentication for public networks
- Keep ESP8266 firmware updated
- Don't expose controller to untrusted networks
- Use WPA2 encryption for WiFi

---

## 📜 License

This project uses open-source libraries:
- **FastLED**: MIT License
- **ESPAsyncWebServer**: LGPL License
- **Arduino**: CC0 Public Domain

---

## 🤝 Contributing

To contribute new patterns or features:

1. Fork the repository
2. Create a feature branch
3. Add your pattern to `patterns.h`
4. Update `constants.h` with pattern name
5. Update `pattern_runner.ino` switch statement
6. Test thoroughly
7. Submit pull request

See [FUTURE_ENHANCEMENTS.md](FUTURE_ENHANCEMENTS.md) for roadmap and development guidelines.

---

## 📚 Additional Resources

- **FastLED Documentation**: http://fastled.io/
- **ESP8266 Docs**: https://arduino-esp8266.readthedocs.io/
- **WS2812B Datasheet**: Search "WS2812B" for pinout and timing
- **Arduino IDE**: https://www.arduino.cc/

---

## 🎉 Changelog

### v2.0 - Modular Architecture Release
- **Added:** `patterns.h` with organized pattern library
- **Added:** `colormanagement.h` for centralized color management
- **Added:** Color picker that works with all patterns
- **Added:** 61 total patterns (was 30)
- **Added:** `FUTURE_ENHANCEMENTS.md` with development roadmap
- **Improved:** Web UI with gradient design and better organization
- **Improved:** Mobile-responsive interface
- **Improved:** Pattern organization by category

### v1.0 - Initial Release
- Basic LED control via web interface
- 30 original patterns
- WiFi connectivity
- Color and speed controls

---

**Version:** 2.0  
**Last Updated:** May 2026  
**Status:** ✅ Production Ready

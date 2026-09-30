#ifndef CONSTANTS_H
#define CONSTANTS_H

// ── Firmware version ─────────────────────────────────────────────────────────
#define FIRMWARE_VERSION "3.0.0"

// ── Hardware ──────────────────────────────────────────────────────────────────
// Change these to match your wiring and strip.
#define LED_PIN        D2          // GPIO4 on NodeMCU/Wemos D1 Mini
#define NUM_LEDS       168
#define LED_TYPE       WS2812B
#define COLOR_ORDER    GRB

// ── Defaults (overridden at runtime by persistent settings) ───────────────────
#define DEFAULT_BRIGHTNESS      128   // 0–255
#define DEFAULT_SPEED           20    // 1–100
#define DEFAULT_HUE             0     // 0–255
#define DEFAULT_SAT             255   // 0–255
#define DEFAULT_PATTERN         0     // index into kPatterns[]
#define DEFAULT_AUTO_CYCLE      true
#define AUTO_CYCLE_INTERVAL_MS  12750UL

// ── Wi-Fi ─────────────────────────────────────────────────────────────────────
#define WIFI_CONNECT_TIMEOUT_MS 15000UL
#define WIFI_RETRY_DELAY_MS     500UL
#define WIFI_RECONNECT_INTERVAL 30000UL  // ms between reconnect attempts
#define DEVICE_HOSTNAME         "fastled"

// ── OTA ───────────────────────────────────────────────────────────────────────
#define OTA_HOSTNAME   DEVICE_HOSTNAME
// OTA password is set in secrets.h as OTA_PASSWORD

// ── Persistence ───────────────────────────────────────────────────────────────
#define SETTINGS_FILE  "/settings.json"
#define SETTINGS_VERSION 1

// ── EEPROM (Wi-Fi runtime credentials) ───────────────────────────────────────
#define EEPROM_SIZE      512
#define EEPROM_ADDR      0
#define WIFI_MAGIC       0xA5A5

// ── Logging ───────────────────────────────────────────────────────────────────
// Set to 1 to enable verbose DEBUG output; 0 for production.
#ifndef DEBUG_LOGGING
#define DEBUG_LOGGING 0
#endif

#define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Serial.printf("[WARN]  " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Serial.printf("[INFO]  " fmt "\n", ##__VA_ARGS__)
#if DEBUG_LOGGING
#define LOG_DEBUG(fmt, ...) Serial.printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) do {} while(0)
#endif

// ── Pattern registry (defined in pattern_runner.ino) ─────────────────────────
// IMPORTANT: indices are stable — never reorder, only append.
extern const int TOTAL_PATTERNS;

// Pattern capability flags (1 byte per pattern, stored in PatternDefinition).
#define PAT_USES_HUE   0x01  // responds to gHue
#define PAT_USES_SAT   0x02  // responds to gSat
#define PAT_USES_SPEED 0x04  // respects showFrame() / gSpeed
#define PAT_USES_PAL   0x08  // uses currentPalette

// Accessors — read directly from kPatterns[], no duplicate arrays.
const char *patternName(int index);
uint8_t     patternFlags(int index);

#endif // CONSTANTS_H

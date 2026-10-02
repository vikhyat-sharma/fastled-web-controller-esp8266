// =============================================================================
// constants.h — Project-wide configuration
// =============================================================================
//
// This is the first file to edit when setting up the project.
// Everything hardware-specific and tuneable lives here.
//
// HOW THIS FILE IS USED
//   Every other file includes constants.h, so a change here affects the
//   whole project. No magic numbers should appear anywhere else.
//
// =============================================================================

#ifndef CONSTANTS_H
#define CONSTANTS_H

// -----------------------------------------------------------------------------
// Firmware version
// Update this string whenever you flash a new build.
// It appears in the web UI, Serial output, and /json/status API response.
// -----------------------------------------------------------------------------
#define FIRMWARE_VERSION "3.0.0"

// -----------------------------------------------------------------------------
// LED hardware
//
// LED_PIN      — which ESP8266 GPIO pin the data wire is connected to.
//                D2 = GPIO4 on NodeMCU / Wemos D1 Mini.
//                Change this if you wired the strip to a different pin.
//
// NUM_LEDS     — total number of LEDs in your strip.
//                Increasing this uses more RAM: each LED costs 3 bytes.
//                168 LEDs = 504 bytes of LED buffer.
//
// LED_TYPE     — the chipset of your strip (WS2812B, WS2811, APA102, etc.)
//
// COLOR_ORDER  — the byte order your strip expects.
//                WS2812B strips are usually GRB, not RGB.
//                If your colors look wrong, try RGB or BGR.
// -----------------------------------------------------------------------------
#define LED_PIN      D2
#define NUM_LEDS     168
#define LED_TYPE     WS2812B
#define COLOR_ORDER  GRB

// -----------------------------------------------------------------------------
// Default values
//
// These are the starting values used on first boot (before any settings are
// saved) and whenever saved settings are missing or corrupted.
//
// All values are overridden at runtime when the user changes them via the
// web UI — they are then saved to flash and restored on the next boot.
//
// BRIGHTNESS   0 = off,  128 = half,  255 = full.
//              Keep below 200 if powering from USB to avoid brownouts.
//
// SPEED        1 = slowest animations,  100 = fastest.
//              Controls the delay between animation frames.
//
// HUE          0–255 maps to the full color wheel (0=red, 85=green, 170=blue).
//
// SAT          0 = white/grey,  255 = fully saturated color.
//
// PATTERN      Index into the pattern list in pattern_runner.ino.
//              0 = Rainbow Cycle (the first pattern).
//
// AUTO_CYCLE   true  = automatically advance to the next pattern every interval.
//              false = stay on the selected pattern until changed manually.
//
// AUTO_CYCLE_INTERVAL_MS
//              How long (milliseconds) each pattern plays before auto-cycling.
//              12750 ms ≈ 12.75 seconds.
// -----------------------------------------------------------------------------
#define DEFAULT_BRIGHTNESS     128
#define DEFAULT_SPEED          20
#define DEFAULT_HUE            0
#define DEFAULT_SAT            255
#define DEFAULT_PATTERN        0
#define DEFAULT_AUTO_CYCLE     true
#define AUTO_CYCLE_INTERVAL_MS 12750UL

// -----------------------------------------------------------------------------
// Wi-Fi
//
// WIFI_CONNECT_TIMEOUT_MS
//              How long (ms) to wait for Wi-Fi on boot before giving up and
//              continuing without a network. LEDs still work without Wi-Fi.
//
// WIFI_RETRY_DELAY_MS
//              Delay between each connection attempt during the boot timeout.
//
// WIFI_RECONNECT_INTERVAL
//              How often (ms) to attempt reconnection if Wi-Fi drops at runtime.
//              Checked in loop() — does not block LED rendering.
//
// DEVICE_HOSTNAME
//              The mDNS hostname. Device is reachable at http://fastled.local
//              on networks that support mDNS (most home routers do).
// -----------------------------------------------------------------------------
#define WIFI_CONNECT_TIMEOUT_MS  15000UL
#define WIFI_RETRY_DELAY_MS      500UL
#define WIFI_RECONNECT_INTERVAL  30000UL
#define DEVICE_HOSTNAME          "fastled"

// -----------------------------------------------------------------------------
// OTA (Over-The-Air firmware updates)
//
// OTA_HOSTNAME  — the hostname Arduino IDE / arduino-cli uses to find the
//                 device for wireless upload. Defaults to DEVICE_HOSTNAME.
//
// OTA_PASSWORD  — set in secrets.h, not here, to keep it out of version control.
//                 If OTA_PASSWORD is not defined, OTA has no password (fine for
//                 a trusted home network; not recommended on shared networks).
// -----------------------------------------------------------------------------
#define OTA_HOSTNAME DEVICE_HOSTNAME

// -----------------------------------------------------------------------------
// Persistent settings (saved to flash via LittleFS)
//
// SETTINGS_FILE     — path of the JSON file on the LittleFS filesystem.
//
// SETTINGS_VERSION  — increment this number whenever you add or remove fields
//                     from the saved settings. The firmware will ignore saved
//                     files with a different version and fall back to defaults,
//                     preventing crashes from stale data after an upgrade.
// -----------------------------------------------------------------------------
#define SETTINGS_FILE    "/settings.json"
#define SETTINGS_VERSION 1

// -----------------------------------------------------------------------------
// EEPROM — used only for storing runtime Wi-Fi credentials
//
// The user can update Wi-Fi credentials through the web UI without reflashing.
// Those credentials are stored in EEPROM separately from the LittleFS settings.
//
// EEPROM_SIZE   — total bytes reserved. 512 is more than enough for one
//                 WiFiCreds struct (~100 bytes).
//
// EEPROM_ADDR   — byte offset where the WiFiCreds struct is written.
//
// WIFI_MAGIC    — a sentinel value written at the start of the struct.
//                 If EEPROM contains a different value, the struct is treated
//                 as uninitialised and compile-time credentials are used instead.
// -----------------------------------------------------------------------------
#define EEPROM_SIZE  512
#define EEPROM_ADDR  0
#define WIFI_MAGIC   0xA5A5

// -----------------------------------------------------------------------------
// Serial logging
//
// Four log levels: ERROR, WARN, INFO, DEBUG.
// ERROR/WARN/INFO are always printed.
// DEBUG is only printed when DEBUG_LOGGING is set to 1.
//
// To enable debug output: add  #define DEBUG_LOGGING 1  before including
// this file, or pass -DDEBUG_LOGGING=1 as a compiler flag.
//
// Usage in code:
//   LOG_INFO("Connected — IP: %s", ip.c_str());
//   LOG_WARN("Settings file missing, using defaults");
//   LOG_ERROR("LittleFS mount failed");
//   LOG_DEBUG("Frame rendered in %u ms", elapsed);  // only prints if DEBUG_LOGGING=1
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Pattern registry — defined in pattern_runner.ino
//
// TOTAL_PATTERNS is computed automatically from the kPatterns[] array size.
// You never need to update it manually.
//
// Pattern capability flags (stored as a bitmask in PatternDefinition.flags):
//
//   PAT_USES_HUE   — the pattern reads gHue, so the Hue slider affects it.
//   PAT_USES_SAT   — the pattern reads gSat, so the Saturation slider affects it.
//   PAT_USES_SPEED — the pattern calls showFrame(), so the Speed slider affects it.
//   PAT_USES_PAL   — the pattern uses currentPalette, so palette changes affect it.
//
// These flags are used by the API (/json/patterns can expose them) and can be
// used by the UI to grey out controls that have no effect on the active pattern.
//
// patternName(i)  — returns the display name of pattern i.
// patternFlags(i) — returns the capability bitmask of pattern i.
// -----------------------------------------------------------------------------
extern const int TOTAL_PATTERNS;

#define PAT_USES_HUE   0x01
#define PAT_USES_SAT   0x02
#define PAT_USES_SPEED 0x04
#define PAT_USES_PAL   0x08

const char *patternName(int index);
uint8_t     patternFlags(int index);

#endif // CONSTANTS_H

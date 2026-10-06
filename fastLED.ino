// =============================================================================
// fastLED.ino — Boot sequence and main loop
// =============================================================================
//
// This is the entry point for the firmware. Arduino calls setup() once on
// boot and then calls loop() repeatedly forever.
//
// WHAT THIS FILE DOES
//   - Defines all global state variables (LED buffer, brightness, speed, etc.)
//   - Connects to Wi-Fi (with a timeout so LEDs work even without a network)
//   - Mounts the LittleFS filesystem and loads saved settings
//   - Starts the OTA update listener
//   - Starts the web server (routes are defined in web_ui.ino)
//   - Runs the animation loop: auto-cycle patterns, render the active pattern,
//     handle Wi-Fi reconnection, and flush changed settings to flash
//
// WHAT THIS FILE DOES NOT DO
//   - Define HTTP routes → see web_ui.ino
//   - Define pattern functions → see patterns.h
//   - Define the pattern registry → see pattern_runner.ino
//   - Define configuration constants → see constants.h
//
// =============================================================================

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "secrets.h"        // Wi-Fi SSID, password, OTA password — never committed
#include "constants.h"      // All configuration constants and #defines
#include "colormanagement.h"// Global state declarations and showFrame() helper
#include "patterns.h"       // All pattern functions

// =============================================================================
// Global state
// =============================================================================
// These variables are declared "extern" in colormanagement.h so that
// patterns.h and web_ui.ino can read and write them without needing to
// include fastLED.ino directly. They are *defined* (memory allocated) here,
// exactly once.

CRGB          leds[NUM_LEDS];           // The LED pixel buffer. 3 bytes per LED.
uint8_t       gHue        = DEFAULT_HUE;
uint8_t       gSat        = DEFAULT_SAT;
uint8_t       gBrightness = DEFAULT_BRIGHTNESS;
uint8_t       gSpeed      = DEFAULT_SPEED;
uint8_t       colorPickerR = 255;       // Last color-picker R value
uint8_t       colorPickerG = 0;
uint8_t       colorPickerB = 0;
bool          useColorPickerOverride = false;
CRGBPalette16 currentPalette = RainbowColors_p;

// Controller state — also read/written by web_ui.ino
int           currentPattern = DEFAULT_PATTERN;
bool          autoCycle      = DEFAULT_AUTO_CYCLE;
unsigned long lastChange     = 0;  // millis() when the pattern last changed

// Internal — not exposed to other files
static unsigned long lastReconnect = 0;  // millis() of last Wi-Fi reconnect attempt
static bool          otaBusy       = false; // true while an OTA upload is in progress

// =============================================================================
// Web server instance
// Declared here so web_ui.ino can register routes via setupWebServer().
// =============================================================================
AsyncWebServer server(80);

// =============================================================================
// EEPROM — runtime Wi-Fi credential storage
// =============================================================================
// The user can update Wi-Fi credentials through the web UI without reflashing.
// Credentials are stored in a small struct in EEPROM. The "magic" field is a
// sentinel: if it doesn't match WIFI_MAGIC, the struct is treated as empty
// and the compile-time credentials from secrets.h are used instead.

struct WiFiCreds {
  uint16_t magic;       // Must equal WIFI_MAGIC for the struct to be considered valid
  char     ssid[32];
  char     password[64];
};

static bool loadWifiFromEEPROM(String &outSsid, String &outPass) {
  WiFiCreds creds;
  EEPROM.get(EEPROM_ADDR, creds);
  if (creds.magic != WIFI_MAGIC) return false;
  // Force null-termination before converting to String, in case EEPROM is corrupt.
  creds.ssid[sizeof(creds.ssid) - 1]         = '\0';
  creds.password[sizeof(creds.password) - 1] = '\0';
  outSsid = String(creds.ssid);
  outPass = String(creds.password);
  return outSsid.length() > 0;
}

// Called by the /wifi/update route in web_ui.ino.
bool saveWifiToEEPROM(const String &newSsid, const String &newPassword) {
  WiFiCreds creds;
  creds.magic = WIFI_MAGIC;
  memset(creds.ssid,     0, sizeof(creds.ssid));
  memset(creds.password, 0, sizeof(creds.password));
  newSsid.toCharArray(creds.ssid,         sizeof(creds.ssid));
  newPassword.toCharArray(creds.password, sizeof(creds.password));
  EEPROM.put(EEPROM_ADDR, creds);
  return EEPROM.commit();
}

// =============================================================================
// Persistent settings — brightness, speed, pattern, etc. saved to flash
// =============================================================================
// Settings are stored as a small JSON file on LittleFS (/settings.json).
// They are loaded once on boot and saved back to flash after any change,
// but with a 5-second debounce to avoid wearing out the flash with rapid
// slider movements.

static void loadSettings() {
  if (!LittleFS.exists(SETTINGS_FILE)) {
    LOG_INFO("No settings file found — using defaults");
    return;
  }
  File f = LittleFS.open(SETTINGS_FILE, "r");
  if (!f) { LOG_WARN("Cannot open settings file"); return; }

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    // File is corrupt or from an incompatible firmware version — ignore it.
    LOG_WARN("Settings parse error (%s) — using defaults", err.c_str());
    return;
  }

  // Only load if the version field matches. This prevents crashes when the
  // saved format changes between firmware versions.
  if ((int)(doc["ver"] | 0) != SETTINGS_VERSION) {
    LOG_WARN("Settings version mismatch — using defaults");
    return;
  }

  gBrightness    = constrain((int)(doc["brightness"] | DEFAULT_BRIGHTNESS), 0, 255);
  gSpeed         = constrain((int)(doc["speed"]      | DEFAULT_SPEED),      1, 100);
  gHue           = constrain((int)(doc["hue"]        | DEFAULT_HUE),        0, 255);
  gSat           = constrain((int)(doc["sat"]        | DEFAULT_SAT),        0, 255);
  currentPattern = constrain((int)(doc["pattern"]    | DEFAULT_PATTERN),    0, TOTAL_PATTERNS - 1);
  autoCycle      = (bool)(doc["autoCycle"] | DEFAULT_AUTO_CYCLE);

  LOG_INFO("Settings loaded — brightness=%d speed=%d pattern=%d autoCycle=%d",
           gBrightness, gSpeed, currentPattern, autoCycle);
}

// Debounced write: markSettingsDirty() is called after any state change.
// The actual flash write only happens after SETTINGS_WRITE_DELAY_MS of
// inactivity, so rapid slider movements coalesce into a single write.
static unsigned long settingsDirtyAt    = 0;
static bool          settingsDirty      = false;
static const unsigned long SETTINGS_WRITE_DELAY_MS = 5000; // 5 seconds

void markSettingsDirty() {
  settingsDirty   = true;
  settingsDirtyAt = millis();
}

static void flushSettingsIfNeeded() {
  if (!settingsDirty) return;
  if (millis() - settingsDirtyAt < SETTINGS_WRITE_DELAY_MS) return;

  File f = LittleFS.open(SETTINGS_FILE, "w");
  if (!f) { LOG_WARN("Cannot write settings file"); return; }

  StaticJsonDocument<256> doc;
  doc["ver"]        = SETTINGS_VERSION;
  doc["brightness"] = gBrightness;
  doc["speed"]      = gSpeed;
  doc["hue"]        = gHue;
  doc["sat"]        = gSat;
  doc["pattern"]    = currentPattern;
  doc["autoCycle"]  = autoCycle;
  serializeJson(doc, f);
  f.close();

  settingsDirty = false;
  LOG_DEBUG("Settings saved to flash");
}

// =============================================================================
// OTA (Over-The-Air firmware updates)
// =============================================================================
// Allows flashing new firmware over Wi-Fi from Arduino IDE or arduino-cli
// without a USB cable. The device appears as a network port in the IDE.
//
// To upload OTA:
//   Arduino IDE: Tools → Port → select the network port named DEVICE_HOSTNAME
//   arduino-cli: arduino-cli upload --port <IP> --fqbn esp8266:esp8266:d1_mini
//
// The OTA password (if set in secrets.h) must be entered when prompted.

static void setupOTA() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

  ArduinoOTA.onStart([]() {
    otaBusy = true;       // Pause LED rendering during upload
    FastLED.clear();
    FastLED.show();
    LOG_INFO("OTA update starting — LEDs paused");
  });

  ArduinoOTA.onEnd([]() {
    LOG_INFO("OTA update complete — rebooting");
    otaBusy = false;
  });

  ArduinoOTA.onError([](ota_error_t e) {
    LOG_ERROR("OTA error %u — resuming normal operation", e);
    otaBusy = false;
  });

  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    yield(); // Keep the watchdog happy during large uploads
  });

  ArduinoOTA.begin();
  LOG_INFO("OTA ready — hostname: %s", OTA_HOSTNAME);
}

// =============================================================================
// Wi-Fi connection
// =============================================================================
// Tries to connect using EEPROM-stored credentials first (set via the web UI),
// then falls back to the compile-time credentials in secrets.h.
//
// The connection attempt has a timeout (WIFI_CONNECT_TIMEOUT_MS) so the device
// doesn't hang forever on boot if the network is unavailable. LEDs work fine
// without Wi-Fi — the web UI just won't be accessible.

static void connectWifi() {
  String storedSsid, storedPass;
  if (loadWifiFromEEPROM(storedSsid, storedPass)) {
    LOG_INFO("Connecting with stored credentials (SSID: %s)", storedSsid.c_str());
    WiFi.begin(storedSsid.c_str(), storedPass.c_str());
  } else {
    LOG_INFO("Connecting with compile-time credentials from secrets.h");
    WiFi.begin(ssid, password);
  }

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < WIFI_CONNECT_TIMEOUT_MS) {
    yield();
    delay(WIFI_RETRY_DELAY_MS);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    LOG_INFO("Wi-Fi connected — IP: %s  RSSI: %d dBm",
             WiFi.localIP().toString().c_str(), WiFi.RSSI());
    if (MDNS.begin(DEVICE_HOSTNAME)) {
      LOG_INFO("mDNS started — open http://%s.local in your browser", DEVICE_HOSTNAME);
    } else {
      LOG_WARN("mDNS failed to start — use the IP address instead");
    }
  } else {
    LOG_WARN("Wi-Fi timed out — running without network (LEDs still work)");
  }
}

// =============================================================================
// setup() — runs once on boot
// =============================================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  LOG_INFO("FastLED Web Controller %s starting", FIRMWARE_VERSION);

  // 1. Start LEDs immediately so the strip shows something during boot.
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000); // Safety power cap
  FastLED.setBrightness(gBrightness);
  FastLED.clear();
  FastLED.show();

  // 2. Mount the filesystem. If it fails, format and retry once.
  //    LittleFS stores settings.json and user palette/favorites files.
  if (!LittleFS.begin()) {
    LOG_WARN("LittleFS mount failed — formatting (first boot or corruption)");
    LittleFS.format();
    if (!LittleFS.begin()) {
      LOG_ERROR("LittleFS unavailable — settings and palettes will not persist");
    }
  } else {
    LOG_INFO("LittleFS mounted");
  }

  // 3. Load saved settings so the strip starts with the user's last state.
  loadSettings();
  FastLED.setBrightness(gBrightness); // Apply loaded brightness

  // 4. Connect to Wi-Fi.
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);  // Let the SDK handle brief dropouts automatically
  WiFi.persistent(false);       // Don't let the SDK write credentials to flash
  WiFi.hostname(DEVICE_HOSTNAME);
  EEPROM.begin(EEPROM_SIZE);
  connectWifi();

  // 5. Start OTA listener.
  setupOTA();

  // 6. Register all HTTP routes and start the web server.
  setupWebServer(); // defined in web_ui.ino

  LOG_INFO("Boot complete — %d patterns available, free heap: %u bytes",
           TOTAL_PATTERNS, ESP.getFreeHeap());
}

// =============================================================================
// loop() — runs repeatedly, as fast as possible
// =============================================================================
// Keep this function fast. Avoid blocking calls here.
// The actual animation work happens inside runCurrentPattern() in pattern_runner.ino.

void loop() {
  // OTA takes priority. While an upload is in progress, skip everything else.
  ArduinoOTA.handle();
  if (otaBusy) { yield(); return; }

  MDNS.update();
  yield();

  // Non-blocking Wi-Fi reconnect.
  // WiFi.setAutoReconnect(true) handles brief dropouts, but if the connection
  // is lost for longer we nudge it manually every WIFI_RECONNECT_INTERVAL ms.
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnect > WIFI_RECONNECT_INTERVAL) {
      lastReconnect = now;
      LOG_WARN("Wi-Fi disconnected — attempting reconnect");
      WiFi.reconnect();
    }
  }

  // Auto-cycle: advance to the next pattern after the interval elapses.
  unsigned long now = millis();
  if (autoCycle && (now - lastChange > AUTO_CYCLE_INTERVAL_MS)) {
    currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
    lastChange     = now;
    markSettingsDirty();
  }

  // Render the active pattern. Defined in pattern_runner.ino.
  runCurrentPattern();

  // Write changed settings to flash (debounced — only writes after 5 s of inactivity).
  flushSettingsIfNeeded();
}

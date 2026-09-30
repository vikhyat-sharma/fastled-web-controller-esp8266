// FastLED Web Controller — ESP8266
// https://github.com/vikhyat-sharma/fastled-web-controller
//
// Boot sequence, Wi-Fi management, OTA, persistent settings, and main loop.

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "constants.h"
#include "colormanagement.h"
#include "patterns.h"

// ── Global state definitions ──────────────────────────────────────────────────
// Declared extern in colormanagement.h; defined exactly once here.
CRGB             leds[NUM_LEDS];
uint8_t          gHue        = DEFAULT_HUE;
uint8_t          gSat        = DEFAULT_SAT;
uint8_t          gBrightness = DEFAULT_BRIGHTNESS;
uint8_t          gSpeed      = DEFAULT_SPEED;
uint8_t          colorPickerR = 255;
uint8_t          colorPickerG = 0;
uint8_t          colorPickerB = 0;
bool             useColorPickerOverride = false;
CRGBPalette16    currentPalette = RainbowColors_p;

// ── Controller state ──────────────────────────────────────────────────────────
int              currentPattern  = DEFAULT_PATTERN;
bool             autoCycle       = DEFAULT_AUTO_CYCLE;
unsigned long    lastChange      = 0;
unsigned long    lastReconnect   = 0;
bool             otaBusy         = false;

// ── Web server ────────────────────────────────────────────────────────────────
AsyncWebServer server(80);

// ── EEPROM Wi-Fi credential storage ──────────────────────────────────────────
struct WiFiCreds {
  uint16_t magic;
  char     ssid[32];
  char     password[64];
};

static bool loadWifiFromEEPROM(String &outSsid, String &outPass) {
  WiFiCreds creds;
  EEPROM.get(EEPROM_ADDR, creds);
  if (creds.magic != WIFI_MAGIC) return false;
  // Ensure null-termination before converting to String.
  creds.ssid[sizeof(creds.ssid) - 1]         = '\0';
  creds.password[sizeof(creds.password) - 1] = '\0';
  outSsid = String(creds.ssid);
  outPass = String(creds.password);
  return outSsid.length() > 0;
}

bool saveWifiToEEPROM(const String &newSsid, const String &newPassword) {
  WiFiCreds creds;
  creds.magic = WIFI_MAGIC;
  memset(creds.ssid,     0, sizeof(creds.ssid));
  memset(creds.password, 0, sizeof(creds.password));
  newSsid.toCharArray(creds.ssid,     sizeof(creds.ssid));
  newPassword.toCharArray(creds.password, sizeof(creds.password));
  EEPROM.put(EEPROM_ADDR, creds);
  return EEPROM.commit();
}

// ── Persistent settings (LittleFS JSON) ──────────────────────────────────────
static void loadSettings() {
  if (!LittleFS.exists(SETTINGS_FILE)) {
    LOG_INFO("No settings file, using defaults");
    return;
  }
  File f = LittleFS.open(SETTINGS_FILE, "r");
  if (!f) { LOG_WARN("Cannot open settings file"); return; }

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    LOG_WARN("Settings JSON parse error: %s — using defaults", err.c_str());
    return;
  }
  if (doc["ver"] | 0) {
    gBrightness    = constrain((int)(doc["brightness"]  | DEFAULT_BRIGHTNESS),  0, 255);
    gSpeed         = constrain((int)(doc["speed"]       | DEFAULT_SPEED),       1, 100);
    gHue           = constrain((int)(doc["hue"]         | DEFAULT_HUE),         0, 255);
    gSat           = constrain((int)(doc["sat"]         | DEFAULT_SAT),         0, 255);
    currentPattern = constrain((int)(doc["pattern"]     | DEFAULT_PATTERN),     0, TOTAL_PATTERNS - 1);
    autoCycle      = doc["autoCycle"] | DEFAULT_AUTO_CYCLE;
    LOG_INFO("Settings loaded: brightness=%d speed=%d pattern=%d", gBrightness, gSpeed, currentPattern);
  }
}

// Debounced save — call this after any state change; actual write is deferred.
static unsigned long settingsDirtyAt = 0;
static bool          settingsDirty   = false;
static const unsigned long SETTINGS_WRITE_DELAY_MS = 5000; // coalesce writes

void markSettingsDirty() {
  settingsDirty   = true;
  settingsDirtyAt = millis();
}

static void flushSettingsIfNeeded() {
  if (!settingsDirty) return;
  if (millis() - settingsDirtyAt < SETTINGS_WRITE_DELAY_MS) return;

  File f = LittleFS.open(SETTINGS_FILE, "w");
  if (!f) { LOG_WARN("Cannot write settings"); return; }

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
  LOG_DEBUG("Settings flushed to flash");
}

// ── OTA setup ─────────────────────────────────────────────────────────────────
static void setupOTA() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

  ArduinoOTA.onStart([]() {
    otaBusy = true;
    FastLED.clear();
    FastLED.show();
    LOG_INFO("OTA update starting");
  });
  ArduinoOTA.onEnd([]() {
    LOG_INFO("OTA update complete");
    otaBusy = false;
  });
  ArduinoOTA.onError([](ota_error_t e) {
    LOG_ERROR("OTA error %u", e);
    otaBusy = false;
  });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    // Yield to keep watchdog happy during large uploads.
    yield();
  });
  ArduinoOTA.begin();
  LOG_INFO("OTA ready");
}

// ── Wi-Fi connection ──────────────────────────────────────────────────────────
static void connectWifi() {
  String storedSsid, storedPass;
  if (loadWifiFromEEPROM(storedSsid, storedPass)) {
    LOG_INFO("Connecting with stored credentials (SSID: %s)", storedSsid.c_str());
    WiFi.begin(storedSsid.c_str(), storedPass.c_str());
  } else {
    LOG_INFO("Connecting with compile-time credentials");
    WiFi.begin(ssid, password);
  }

  unsigned long t = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t < WIFI_CONNECT_TIMEOUT_MS) {
    yield();
    delay(WIFI_RETRY_DELAY_MS);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    LOG_INFO("WiFi connected — IP: %s  RSSI: %d dBm",
             WiFi.localIP().toString().c_str(), WiFi.RSSI());
    if (MDNS.begin(DEVICE_HOSTNAME)) {
      LOG_INFO("mDNS: http://%s.local", DEVICE_HOSTNAME);
    } else {
      LOG_WARN("mDNS setup failed");
    }
  } else {
    LOG_WARN("WiFi connection timed out — continuing without network");
  }
}

// ── setup() ───────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(100);
  LOG_INFO("FastLED Web Controller %s booting", FIRMWARE_VERSION);

  // LEDs first — show something immediately.
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
  FastLED.setBrightness(gBrightness);
  FastLED.clear();
  FastLED.show();

  // Filesystem.
  if (!LittleFS.begin()) {
    LOG_WARN("LittleFS mount failed — formatting");
    LittleFS.format();
    if (!LittleFS.begin()) {
      LOG_ERROR("LittleFS unavailable after format");
    }
  } else {
    LOG_INFO("LittleFS mounted");
  }

  // Load persisted settings before Wi-Fi so pattern/brightness are correct.
  loadSettings();
  FastLED.setBrightness(gBrightness);

  // Wi-Fi.
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.hostname(DEVICE_HOSTNAME);
  EEPROM.begin(EEPROM_SIZE);
  connectWifi();

  // OTA.
  setupOTA();

  // Web server routes.
  setupWebServer();

  LOG_INFO("Boot complete — %d patterns, free heap: %u bytes",
           TOTAL_PATTERNS, ESP.getFreeHeap());
}

// ── loop() ────────────────────────────────────────────────────────────────────
void loop() {
  // OTA takes priority — skip LED rendering while updating.
  ArduinoOTA.handle();
  if (otaBusy) { yield(); return; }

  MDNS.update();
  yield();

  // Non-blocking Wi-Fi reconnect.
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnect > WIFI_RECONNECT_INTERVAL) {
      lastReconnect = now;
      LOG_WARN("WiFi lost — attempting reconnect");
      WiFi.reconnect();
    }
  }

  // Auto-cycle.
  unsigned long now = millis();
  if (autoCycle && (now - lastChange > AUTO_CYCLE_INTERVAL_MS)) {
    currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
    lastChange = now;
    markSettingsDirty();
  }

  // Render current pattern.
  runCurrentPattern();

  // Flush settings to flash if dirty and debounce period has elapsed.
  flushSettingsIfNeeded();
}

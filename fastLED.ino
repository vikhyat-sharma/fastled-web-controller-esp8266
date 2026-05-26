#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include "secrets.h"
#include "constants.h"
#include "colormanagement.h"
#include "patterns.h"

#include <EEPROM.h>
#include <fauxmoESP.h>

// EEPROM layout
#define EEPROM_SIZE 512
#define EEPROM_ADDR 0
const uint16_t WIFI_MAGIC = 0xA5A5;

struct WiFiCreds {
  uint16_t magic;
  char ssid[32];
  char password[64];
};

fauxmoESP fauxmo;

// Load/save helpers (defined below)
bool loadWifiFromEEPROM(String &outSsid, String &outPass);
bool saveWifiToEEPROM(const String &newSsid, const String &newPassword);

// Implementations
bool loadWifiFromEEPROM(String &outSsid, String &outPass) {
  WiFiCreds creds;
  EEPROM.get(EEPROM_ADDR, creds);
  if (creds.magic != WIFI_MAGIC) return false;
  outSsid = String(creds.ssid);
  outPass = String(creds.password);
  return true;
}

bool saveWifiToEEPROM(const String &newSsid, const String &newPassword) {
  WiFiCreds creds;
  creds.magic = WIFI_MAGIC;
  memset(creds.ssid, 0, sizeof(creds.ssid));
  memset(creds.password, 0, sizeof(creds.password));
  newSsid.toCharArray(creds.ssid, sizeof(creds.ssid));
  newPassword.toCharArray(creds.password, sizeof(creds.password));
  EEPROM.put(EEPROM_ADDR, creds);
  return EEPROM.commit();
}

AsyncWebServer server(80);

CRGB leds[NUM_LEDS];
bool autoCycle = true;
uint8_t currentPattern = 0;
unsigned long lastChange = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(gBrightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
  FastLED.clear();
  FastLED.show();

  // Initialize EEPROM and try to load stored WiFi credentials
  EEPROM.begin(EEPROM_SIZE);
  String runtimeSsid, runtimePass;
  if (loadWifiFromEEPROM(runtimeSsid, runtimePass)) {
    Serial.println("Using stored WiFi credentials from EEPROM");
    WiFi.begin(runtimeSsid.c_str(), runtimePass.c_str());
  } else {
    Serial.println("Using compile-time WiFi credentials from secrets.h");
    WiFi.begin(ssid, password);
  }

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT_MS) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("fastled")) {
      Serial.println("mDNS ready: http://fastled.local");
    } else {
      Serial.println("mDNS setup failed.");
    }
  } else {
    Serial.println("\n⚠️ Failed to connect to WiFi within timeout.");
  }

  setupWebServer();

  // Setup Fauxmo (Alexa emulation) - expose pattern names + controls
  fauxmo.createServer(true);
  fauxmo.setPort(56700); // This is the default
  fauxmo.enable(true);

  // Add device to control power and navigation
  fauxmo.addDevice("FastLED");
  fauxmo.addDevice("Next Pattern");
  fauxmo.addDevice("Previous Pattern");

  // Add one device per pattern so users can say "Turn on <pattern name>"
  for (int i = 0; i < TOTAL_PATTERNS; i++) {
    fauxmo.addDevice(patternNames[i]);
  }

  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state) {
    String dname(device_name);
    // Power control
    if (dname == "FastLED") {
      if (state) {
        // turn on (resume patterns)
        autoCycle = false;
      } else {
        // turn off - clear LEDs
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        autoCycle = false;
      }
      return;
    }

    // Navigation
    if (dname == "Next Pattern" && state) {
      currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
      lastChange = millis();
      return;
    }
    if (dname == "Previous Pattern" && state) {
      currentPattern = (currentPattern - 1 + TOTAL_PATTERNS) % TOTAL_PATTERNS;
      lastChange = millis();
      return;
    }

    // Match pattern names (turning a pattern device ON switches to it)
    if (state) {
      for (int i = 0; i < TOTAL_PATTERNS; i++) {
        if (dname == String(patternNames[i])) {
          currentPattern = i;
          lastChange = millis();
          break;
        }
      }
    }
  });
}

void loop() {
  MDNS.update();
  fauxmo.handle();
  unsigned long now = millis();

  if (autoCycle) {
    if (now - lastChange > AUTO_CYCLE_INTERVAL_MS) {
      currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
      lastChange = now;
    }
  }

  runCurrentPattern();
}

// ============================================================
// All pattern functions are now organized in patterns.h
// Color management functions are in colormanagement.h
// Web server setup is in web_ui.ino
// ============================================================

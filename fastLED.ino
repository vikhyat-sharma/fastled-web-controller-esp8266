#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include "secrets.h"
#include "constants.h"
#include "colormanagement.h"
#include "patterns.h"

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
  WiFi.begin(ssid, password);
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
}

void loop() {
  MDNS.update();
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

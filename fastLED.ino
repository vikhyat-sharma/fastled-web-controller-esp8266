#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include "secrets.h"

AsyncWebServer server(80);

#define LED_PIN D2
#define NUM_LEDS 168
#define BRIGHTNESS 128
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
uint8_t gSat = 255;
unsigned long lastChange = 0;
uint8_t currentPattern = 0;
uint8_t gBrightness = BRIGHTNESS;
uint8_t gSpeed = 20;
bool autoCycle = true;
CRGBPalette16 currentPalette = RainbowColors_p;

uint8_t customColorR = 255;
uint8_t customColorG = 0;
uint8_t customColorB = 0;

const char *patternNames[] = {
  "Rainbow Cycle",
  "Moving Dot",
  "Breathing Effect",
  "Confetti",
  "Sinelon",
  "Juggle",
  "Twinkle",
  "Rainbow Glitter",
  "Beat Wave",
  "Fire Effect",
  "Ripple",
  "Gradient Wave",
  "Rainbow Pulse",
  "Plasma",
  "Noise Rainbow",
  "Sunrise",
  "Aurora Borealis",
  "Ocean Current",
  "Galaxy Swirl",
  "Storm Pulse",
  "Hyperspace Tunnel",
  "Aura Glow",
  "Wave Glide",
  "Comet Tail",
  "Matrix Rain",
  "Lava Flow",
  "Vortex Spin",
  "Electric Pulse",
  "Candy Cane Twist",
  "Palette Cycle"
};

const int TOTAL_PATTERNS = sizeof(patternNames) / sizeof(patternNames[0]);

uint16_t scaledDelay(uint16_t baseDelay) {
  uint8_t speed = constrain(gSpeed, 1, 100);
  uint16_t minDelay = baseDelay / 4;
  if (minDelay < 1) minDelay = 1;
  uint16_t maxDelay = baseDelay * 3;
  return map(speed, 1, 100, maxDelay, minDelay);
}

void showFrame(uint16_t baseDelay) {
  FastLED.setBrightness(gBrightness);
  FastLED.show();
  delay(scaledDelay(baseDelay));
}

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
  const unsigned long timeout = 10000;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
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
    if (now - lastChange > 12750) {
      currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;
      lastChange = now;
    }
  }

  runCurrentPattern();
}

void paletteCycle() {
  static uint8_t startIndex = 0;
  startIndex += 1;
  fill_palette(leds, NUM_LEDS, startIndex, 255 / NUM_LEDS, currentPalette, 255, LINEARBLEND);
  showFrame(18);
}

void rainbowCycle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  gHue++;
  showFrame(20);
}

void movingDot() {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = CHSV(gHue, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  gHue++;
  showFrame(16);
}

void breathingEffect() {
  static uint8_t brightness = 0;
  static int delta = 5;
  fill_solid(leds, NUM_LEDS, CHSV(gHue, gSat, brightness));
  brightness += delta;
  if (brightness == 0 || brightness >= gBrightness) delta = -delta;
  showFrame(16);
}

void confetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), gSat, 255);
  showFrame(14);
}

void confetti2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  leds[random16(NUM_LEDS)] += CHSV(gHue + random8(64), 200, 255);
  FastLED.show();
  delay(20);
}

void sinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, gSat, 255);
  showFrame(14);
}

void juggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(gHue + i * 32, gSat, 255);
  }
  showFrame(14);
}

void sparkle() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
  FastLED.show();
  delay(20);
}

void twinkle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 20)
      leds[i] = CHSV(gHue + random8(96), gSat, 255);
    else
      leds[i].fadeToBlackBy(20);
  }
  showFrame(24);
}

void lightning() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  if (random8() < 80) {
    int start = random16(NUM_LEDS);
    int len = random8(10, 50);
    for (int i = start; i < start + len && i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
  }
  FastLED.show();
  delay(30);
}

void rainbowGlitter() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  if (random8() < 80) leds[random16(NUM_LEDS)] += CRGB::White;
  gHue++;
  showFrame(20);
}

void colorWipe(CRGB color) {
  static int pos = 0;
  leds[pos] = color;
  FastLED.show();
  pos++;
  if (pos >= NUM_LEDS) pos = 0;
  delay(5);
}

void theaterChase(CRGB color) {
  static int q = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = q; i < NUM_LEDS; i += 3) {
    leds[i] = color;
  }
  FastLED.show();
  q = (q + 1) % 3;
  delay(50);
}

void fadeToBlack() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  FastLED.show();
  delay(20);
}

void randomStrobe() {
  if (random8() < 60)
    fill_solid(leds, NUM_LEDS, CRGB::White);
  else
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(50);
}

void randomColorFill() {
  fill_solid(leds, NUM_LEDS, CHSV(random8(), 255, 255));
  FastLED.show();
  delay(500);
}

void rainbowFill() {
  fill_rainbow(leds, NUM_LEDS, gHue, 3);
  FastLED.show();
  gHue += 5;
  delay(30);
}

void beatWave() {
  CRGBPalette16 palette = PartyColors_p;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t idx = beatsin8(6, 0, 255) + (i * 10);
    uint8_t val = sin8(idx);
    leds[i] = CHSV(gHue + idx, gSat, val);
  }
  showFrame(18);
}

void bouncingBalls() {
  static int ballPos[5] = { 0 };
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < 5; i++) {
    ballPos[i] = (ballPos[i] + random8(1, 5)) % NUM_LEDS;
    leds[ballPos[i]] = CHSV(random8(), 255, 255);
  }
  FastLED.show();
  delay(50);
}

void meteorRain(CRGB color) {
  fadeToBlackBy(leds, NUM_LEDS, 64);
  leds[random(NUM_LEDS)] = color;
  FastLED.show();
  delay(30);
}

void rainbowTwinkle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 10; i++) {
    leds[random(NUM_LEDS)] = CHSV(random8(), 200, 255);
  }
  FastLED.show();
  delay(30);
}

void fireEffect() {
  static byte heat[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((55 * 10) / NUM_LEDS) + 2));
  }
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  if (random8() < 120) {
    heat[0] = qadd8(heat[0], random8(160, 255));
  }
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor(heat[j]);
  }
  showFrame(18);
}

void policeLights() {
  static bool state = false;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (state && (i % 2 == 0)) leds[i] = CRGB::Blue;
    else if (!state && (i % 2 == 1)) leds[i] = CRGB::Red;
  }
  state = !state;
  FastLED.show();
  delay(100);
}

void ripple() {
  static int center = 0;
  static int step = -1;
  if (step == -1) {
    center = random(NUM_LEDS);
    step = 0;
  }
  fadeToBlackBy(leds, NUM_LEDS, 64);
  leds[center] = CRGB::White;
  for (int i = 1; i < NUM_LEDS / 2; i++) {
    if (center + i < NUM_LEDS) leds[center + i].fadeToBlackBy(80);
    if (center - i >= 0) leds[center - i].fadeToBlackBy(80);
  }
  step++;
  if (step > NUM_LEDS / 2) step = -1;
  showFrame(24);
}

void colorExplosion() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  int center = random(NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    int distance = abs(center - i);
    if (distance < 10)
      leds[i] = CHSV(gHue + (distance * 10), 255, 255 - (distance * 20));
  }
  FastLED.show();
  delay(60);
}

void smoothChase() {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < 5; i++) {
    leds[(pos + i * 5) % NUM_LEDS] = CHSV(gHue + i * 30, 255, 255);
  }
  pos = (pos + 1) % NUM_LEDS;
  FastLED.show();
  delay(20);
}

void softTwinkle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 30)
      leds[i] = CHSV(random8(), 255, 255);
    else
      leds[i].fadeToBlackBy(20);
  }
  FastLED.show();
  delay(30);
}

void sparkleBurst() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  int pos = random(NUM_LEDS);
  leds[pos] = CHSV(random8(), 200, 255);
  if (pos > 0) leds[pos - 1] = CHSV(random8(), 200, 150);
  if (pos < NUM_LEDS - 1) leds[pos + 1] = CHSV(random8(), 200, 150);
  FastLED.show();
  delay(15);
}

void gradientWave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, 255);
  }
  showFrame(20);
}

void colorSweep() {
  static uint8_t sweepHue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(sweepHue, 255, 255);
  }
  sweepHue++;
  FastLED.show();
  delay(30);
}

void softMeteor() {
  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 40);
  leds[pos] = CHSV(random8(), 255, 255);
  for (int i = 1; i < 6; i++) {
    if (pos - i >= 0) leds[pos - i].fadeToBlackBy(64);
    if (pos + i < NUM_LEDS) leds[pos + i].fadeToBlackBy(64);
  }
  pos = (pos + 1) % NUM_LEDS;
  FastLED.show();
  delay(30);
}

void rainbowPulse() {
  static uint8_t pulseBrightness = 0;
  static int delta = 5;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, pulseBrightness);
  }
  pulseBrightness += delta;
  if (pulseBrightness == 0 || pulseBrightness == 255) delta = -delta;
  showFrame(18);
}

void colorStrobe() {
  static bool flashState = false;
  fill_solid(leds, NUM_LEDS, flashState ? CRGB(CHSV(random8(), 255, 255)) : CRGB::Black);
  flashState = !flashState;
  FastLED.show();
  delay(100);
}

void beatChase() {
  fadeToBlackBy(leds, NUM_LEDS, 32);
  leds[beatsin16(12, 0, NUM_LEDS - 1)] = CHSV(random8(), 255, 255);
  FastLED.show();
  delay(20);
}

void pastelRainbow() {
  fill_rainbow(leds, NUM_LEDS, gHue, 4);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(180);  // soften intensity for pastel look
  }
  FastLED.show();
  gHue += 1;
  delay(30);
}

void layeredPulse() {
  uint8_t level = beatsin8(7, 50, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 2), 255, level);
  }
  FastLED.show();
  delay(20);
}

void meteorRain() {
  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 64);
  leds[pos] = CHSV(random8(), 255, 255);
  if (pos > 0) leds[pos - 1].fadeToBlackBy(180);
  if (pos < NUM_LEDS - 1) leds[pos + 1].fadeToBlackBy(180);
  pos = (pos + 1) % NUM_LEDS;
  FastLED.show();
  delay(25);
}

void colorWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 5), 255, 255);
  }
  FastLED.show();
  delay(20);
}

void lightning2() {
  if (random8() < 20) {
    int flashCount = random(3, 6);
    for (int i = 0; i < flashCount; i++) {
      fill_solid(leds, NUM_LEDS, CRGB::White);
      FastLED.show();
      delay(random(20, 70));
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(random(50, 150));
    }
  }
}

void plasma() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, sin8(i * 8 + millis() / 3));
  }
  showFrame(20);
}

void sinelon2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 255);
  FastLED.show();
  delay(15);
}

void glitterRainbow() {
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  if (random8() < 80) {
    leds[random16(NUM_LEDS)] = CRGB::White;
  }
  FastLED.show();
  delay(20);
}

void colorFade() {
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
  FastLED.show();
  gHue++;
  delay(20);
}

void bouncingBalls2() {
  static uint8_t ballPos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[ballPos] = CHSV(gHue + ballPos * 2, 255, 255);
  ballPos++;
  if (ballPos >= NUM_LEDS) ballPos = 0;
  FastLED.show();
  delay(10);
}

void fireworks() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  for (int i = 0; i < 2; i++) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(), 255, 255);
    if (pos > 0) leds[pos - 1] = CRGB::White;
    if (pos < NUM_LEDS - 1) leds[pos + 1] = CRGB::White;
  }
  FastLED.show();
  delay(30);
}

void juggle2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  FastLED.show();
  delay(20);
}

void chaseRainbow() {
  fadeToBlackBy(leds, NUM_LEDS, 64);
  int pos = beatsin16(10, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(gHue++, 255, 255);
  FastLED.show();
  delay(20);
}

void noiseRainbow() {
  static uint16_t x = 0, y = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(x + i * 50, y);
    leds[i] = CHSV(gHue + noise, gSat, 255);
  }
  y += 20;
  showFrame(24);
}

void colorfulGlitter() {
  if (random8() < 60) {
    leds[random16(NUM_LEDS)] += CHSV(random8(), 255, 255);
  }
  FastLED.show();
  delay(20);
}

void candleFlicker() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, random8(100, 255), random8(0, 50));
  }
  FastLED.show();
  delay(50);
}

void sunrise() {
  static uint8_t brightness = 0;
  fill_solid(leds, NUM_LEDS, CHSV(gHue + 10, gSat, brightness));
  brightness = qadd8(brightness, 1);
  if (brightness >= 255) brightness = 0;
  showFrame(26);
}

void swirl() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 4) + sin8(millis() / 4), 255, 255);
  }
  FastLED.show();
  delay(20);
}

void pacific() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160 + sin8(i * 4 + millis() / 6), 255, 200);
  }
  FastLED.show();
  delay(25);
}

void pulseWaveChase() {
  static uint16_t pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  leds[(pos + NUM_LEDS / 2) % NUM_LEDS] = CHSV(gHue, 255, 255);
  leds[pos] = CHSV(gHue + 128, 255, 255);
  pos = (pos + 1) % NUM_LEDS;
  FastLED.show();
  delay(20);
}

void fireEffect2() {
  static byte heat[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((55 * 10) / NUM_LEDS) + 2));
  }
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  if (random8() < 60) {
    heat[0] = qadd8(heat[0], random8(160, 255));
  }
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor(heat[j]);
  }
  FastLED.show();
  delay(20);
}

void rainbowBeat() {
  uint8_t beatA = beatsin8(17, 0, 255);
  uint8_t beatB = beatsin8(13, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * beatA / 50), 255, beatB);
  }
  FastLED.show();
  delay(20);
}

void dazzle() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  for (int i = 0; i < 5; i++) {
    leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
  }
  FastLED.show();
  delay(30);
}

void perlinNoiseColors() {
  static uint16_t x = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 30, x);
    leds[i] = CHSV(noise, 255, 255);
  }
  x += 5;
  FastLED.show();
  delay(20);
}

void lavaLamp() {
  static uint8_t hueBase = 0;
  fill_gradient(leds, NUM_LEDS, CHSV(hueBase, 255, 255), CHSV(hueBase + 64, 255, 255));
  hueBase++;
  FastLED.show();
  delay(40);
}

void noiseGradient() {
  static uint16_t noiseX = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(noiseX + i * 10, millis() / 5);
    leds[i] = CHSV(noise, 200, 255);
  }
  noiseX += 3;
  FastLED.show();
  delay(30);
}

void radiantWaves() {
  uint8_t wave = sin8(millis() / 3);
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, wave));
  FastLED.show();
  delay(20);
}

void colorfulSinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(random8(), 255, 255);
  FastLED.show();
  delay(20);
}

void movingSpot() {
  static uint16_t pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 25);
  leds[pos] = CHSV(gHue, 255, 255);
  pos = (pos + 1) % NUM_LEDS;
  FastLED.show();
  delay(10);
}

void twinkle2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 50) {
    leds[random16(NUM_LEDS)] = CHSV(random8(), 200, 255);
  }
  FastLED.show();
  delay(20);
}

void runningColors() {
  static uint8_t hueShift = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hueShift + (i * 3), 255, 255);
  }
  hueShift += 2;
  FastLED.show();
  delay(30);
}

void glitchFlicker() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(random8(), 255, random8(128, 255));
  }
  FastLED.show();
  delay(random8(50, 200));
}

void theaterChaseRainbow() {
  static int j = 0;
  static int q = 0;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  for (int i = q; i < NUM_LEDS; i += 3) {
    leds[i] = CHSV(j, 200, 255);
  }
  FastLED.show();
  delay(50);
  q++;
  if (q >= 3) {
    q = 0;
    j += 8;
  }
}

void softStrobe() {
  static bool flash = false;
  fill_solid(leds, NUM_LEDS, flash ? CRGB(CHSV(gHue, 255, 255)) : CRGB::Black);
  flash = !flash;
  FastLED.show();
  delay(flash ? 50 : 300);
}

void starfield() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  if (random8() < 30) {
    leds[random16(NUM_LEDS)] = CRGB::White;
  }
  FastLED.show();
  delay(20);
}

void plasmaWave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(sin8(i * 8 + millis() / 3), 255, 255);
  }
  FastLED.show();
  delay(20);
}

void cylonBounce() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[pos] = CRGB::Red;
  FastLED.show();
  delay(15);
  pos += dir;
  if (pos == NUM_LEDS - 1 || pos == 0) dir = -dir;
}

void partyPalette() {
  static uint8_t startIndex = 0;
  startIndex++;
  fill_palette(leds, NUM_LEDS, startIndex, 16, PartyColors_p, 255, LINEARBLEND);
  FastLED.show();
  delay(20);
}

void confettiPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[random16(NUM_LEDS)] += CHSV(gHue + random8(64), 200, 255);
  FastLED.show();
  delay(15);
}

void twinkleFox() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 40) {
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(random8(), 200, 255);
  }
}

void dreamyFade() {
  static uint8_t hue = 0;
  fill_solid(leds, NUM_LEDS, CHSV(hue, 100, 180));
  EVERY_N_MILLISECONDS(50) {
    hue++;
  }
}
void colorRipple() {
  static uint8_t ripplePos = 0;
  static uint8_t center = NUM_LEDS / 2;
  fadeToBlackBy(leds, NUM_LEDS, 40);

  leds[center + ripplePos] = CHSV(random8(), 255, 255);
  leds[center - ripplePos] = CHSV(random8(), 255, 255);

  ripplePos++;
  if (center + ripplePos >= NUM_LEDS || center - ripplePos < 0) {
    ripplePos = 0;
  }
}
void rainbowSnake() {
  static uint8_t startIndex = 0;
  startIndex++;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(startIndex + i * 4, 255, 255);
  }
}
void auroraBorealis() {
  static uint8_t baseHue = 90;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 20, millis() / 4);
    leds[i] = CHSV(gHue + baseHue + noise / 3, gSat, noise);
  }
  showFrame(22);
}
void forestFirefly() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 30) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(70 + random8(20), 200, 255);
  }
}
void pulseWave() {
  uint8_t brightness = beatsin8(8, 50, 255);
  fill_solid(leds, NUM_LEDS, CHSV(160, 255, brightness));
}
void oceanCurrent() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8(i * 8 + millis() / 4);
    leds[i] = CHSV(gHue + 140 + wave / 12, gSat, wave);
  }
  showFrame(22);
}
void galaxySwirl() {
  static uint8_t centerHue = 180;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t angle = (i * 4 + millis() / 8) % 255;
    leds[i] = CHSV(gHue + centerHue + angle, gSat, sin8(angle));
  }
  centerHue += 1;
  showFrame(22);
}
void sciFiScanner() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  leds[pos] = CRGB::Red;
  pos += dir;
  if (pos == NUM_LEDS - 1 || pos == 0) dir = -dir;
}
void bioLuminescentShimmer() {
  fadeToBlackBy(leds, NUM_LEDS, 15);
  if (random8() < 20) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(160 + random8(20), 200, 255);
  }
}
void sunsetGlow() {
  static uint8_t hueShift = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = map(i, 0, NUM_LEDS - 1, 10, 170);
    leds[i] = CHSV(hue + hueShift, 180, 255);
  }
  hueShift += 1;
}
void flameTunnel() {
  static uint8_t offset = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((offset + i * 4) % 255, 255, sin8(i * 8 + offset));
  }
  offset += 3;
}
void binaryWave() {
  static uint8_t tick = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    bool on = bitRead(i + tick, 0);
    leds[i] = on ? CRGB::Green : CRGB::Black;
  }
  tick++;
}
void rainbowSparkle() {
  static uint8_t baseHue = 0;
  baseHue++;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(baseHue + i * 5, 200, 200);
  }

  if (random8() < 20) {
    int sparkle = random16(NUM_LEDS);
    leds[sparkle] = CRGB::White;
  }
}
void staticStarfield() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  if (random8() < 10) {
    int star = random16(NUM_LEDS);
    leds[star] = CHSV(0, 0, random8(180, 255));
  }
}
void emberFireflies() {
  fadeToBlackBy(leds, NUM_LEDS, 20);

  if (random8() < 40) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(10, 25), 200, random8(150, 255));
  }
}
void stormPulse() {
  static uint8_t wave = 0;
  wave += 2;
  fill_solid(leds, NUM_LEDS, CHSV(gHue + 160, gSat, sin8(wave)));

  if (random8() < 8) {
    int strikeStart = random16(NUM_LEDS - 10);
    for (int i = 0; i < 10; i++) {
      leds[strikeStart + i] = CRGB::White;
    }
  }
  showFrame(24);
}
void waveformShimmer() {
  static uint8_t offset = 0;
  offset += 1;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(180, 50, sin8(i * 6 + offset) / 2);
  }
}
void neonStreaks() {
  fadeToBlackBy(leds, NUM_LEDS, 30);

  if (random8() < 15) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(), 255, 255);
  }

  for (int i = NUM_LEDS - 1; i > 0; i--) {
    leds[i] |= leds[i - 1].fadeToBlackBy(100);
  }
}
void hyperspaceTunnel() {
  static uint8_t zoom = 0;
  zoom += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((gHue + zoom + i * 8) % 255, gSat, sin8(i * 4 + zoom));
  }
  showFrame(16);
}
void cometTail() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  static uint8_t pos = 0;
  leds[pos] = CHSV(gHue++, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  showFrame(18);
}
void auraGlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t glow = sin8(millis() / 10 + i * 4);
    leds[i] = CHSV(gHue + i * 2, gSat, glow);
  }
  gHue++;
  showFrame(22);
}
void smoothWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((sin8(i * 2 + millis() / 10)), 255, 200);
  }
}
void shimmer() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 20) {
      leds[i] = CHSV(random8(), 200, random8(100, 255));
    } else {
      leds[i].fadeToBlackBy(20);
    }
  }
}
void dotTrail() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  static uint8_t dotPos = 0;
  leds[dotPos] = CHSV(gHue++, 255, 255);
  dotPos = (dotPos + 1) % NUM_LEDS;
}
void auroraDrift() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((millis() / 20 + i * 3) % 255, 150, 255 - abs((NUM_LEDS / 2) - i) * 5);
  }
}
void chaseFade() {
  fadeToBlackBy(leds, NUM_LEDS, 25);
  static uint8_t pos = 0;
  leds[pos] = CHSV(gHue, 255, 255);
  pos = (pos + 1) % NUM_LEDS;
}
void pixelPop() {
  if (random8() < 40) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(), 255, 255);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(15);
  }
}
void centerPulse() {
  int center = NUM_LEDS / 2;
  uint8_t brightness = sin8(millis() / 5);
  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(i - center);
    leds[i] = CHSV(gHue, 255, brightness - dist * 8);
  }
}
void mysticFlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 4 + millis() / 8) % 255, 200, 255);
  }
}
void glitterFade() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 80) {
    leds[random16(NUM_LEDS)] += CHSV(random8(), 200, 255);
  }
}

void noiseLava() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 10, millis() / 5);
    leds[i] = CHSV(noise, 255, noise);
  }
}

void rippleStars() {
  static int center = 0;
  static uint8_t color = 0;
  static uint8_t step = 0;
  fadeToBlackBy(leds, NUM_LEDS, 40);
  if (step == 0) {
    center = random(NUM_LEDS);
    color = random8();
    step = 1;
  }
  if (step < 8) {
    int left = center - step;
    int right = center + step;
    if (left >= 0) leds[left] = CHSV(color, 255, 192 / step);
    if (right < NUM_LEDS) leds[right] = CHSV(color, 255, 192 / step);
    step++;
  } else {
    step = 0;
  }
}

void bounceComets() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[pos] = CHSV(millis() / 10, 255, 255);
  pos += dir;
  if (pos <= 0 || pos >= NUM_LEDS - 1) dir = -dir;
}

void auroraWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(96 + sin8(millis() / 20 + i * 2) / 8, 255, sin8(millis() / 10 + i * 3));
  }
}

void shimmerCurtain() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160, 255, random8() > 240 ? 255 : 0);
  }
}

void fallingStars() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  int pos = random(NUM_LEDS);
  leds[pos] = CHSV(0, 0, 255);
}

void pastelTwinkle() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  if (random8() < 60) {
    int pos = random(NUM_LEDS);
    leds[pos] = CHSV(random8(), 100, 255);
  }
}

void lightningStorm() {
  EVERY_N_MILLISECONDS(1000 + random16(3000)) {
    for (int i = 0; i < random(1, 4); i++) {
      fill_solid(leds, NUM_LEDS, CHSV(0, 0, 255));
      FastLED.show();
      delay(random(30, 80));
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(random(50, 150));
    }
  }
}

void waveGlide() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = sin8(i * 3 + millis() / 5);
    leds[i] = CHSV(gHue + index, gSat, 255);
  }
  showFrame(20);
}

void matrixRain() {
  fadeToBlackBy(leds, NUM_LEDS, 35);
  for (int i = 0; i < NUM_LEDS; i += 8) {
    if (random8() < 90) {
      int pos = (i + random8(8)) % NUM_LEDS;
      leds[pos] = CHSV(96 + random8(20), gSat, 255);
    }
  }
  showFrame(20);
}

void lavaFlow() {
  static uint16_t offset = 0;
  offset += 6;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t n = inoise8(i * 24, offset);
    uint8_t hue = 8 + scale8(n, 28);
    leds[i] = CHSV(gHue + hue, gSat, n);
  }
  showFrame(18);
}

void vortexSpin() {
  static uint8_t spin = 0;
  spin += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8((i * 11) + spin);
    leds[i] = CHSV(gHue + wave, gSat, wave);
  }
  showFrame(16);
}

void electricPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 45);
  uint8_t beat = beatsin8(24, 0, NUM_LEDS - 1);
  leds[beat] = CHSV(gHue + 160, gSat, 255);
  if (beat > 0) leds[beat - 1] = CHSV(gHue + 160, gSat, 180);
  if (beat < NUM_LEDS - 1) leds[beat + 1] = CHSV(gHue + 160, gSat, 180);
  showFrame(12);
}

void candyCaneTwist() {
  static uint8_t phase = 0;
  phase += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    bool stripe = ((i + phase) / 6) % 2 == 0;
    leds[i] = stripe ? CHSV(gHue, gSat, 255) : CHSV(gHue + 128, gSat, 255);
  }
  showFrame(24);
}

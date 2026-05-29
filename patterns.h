#ifndef PATTERNS_H
#define PATTERNS_H

// ============================================================
// CATEGORY: Rainbow & Color Cycling Patterns
// ============================================================

void rainbowCycle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  gHue++;
  showFrame(20);
}

void rainbowGlitter() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  if (random8() < 80) leds[random16(NUM_LEDS)] += CRGB::White;
  gHue++;
  showFrame(20);
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

void colorSweep() {
  static uint8_t sweepHue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(sweepHue, 255, 255);
  }
  sweepHue++;
  FastLED.show();
  delay(30);
}

void colorFade() {
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
  FastLED.show();
  gHue++;
  delay(20);
}

void colorWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 5), 255, 255);
  }
  FastLED.show();
  delay(20);
}

void gradientWave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, 255);
  }
  showFrame(20);
}

void paletteCycle() {
  static uint8_t startIndex = 0;
  startIndex += 1;
  fill_palette(leds, NUM_LEDS, startIndex, 255 / NUM_LEDS, currentPalette, 255, LINEARBLEND);
  showFrame(18);
}

// ============================================================
// CATEGORY: Movement & Chase Patterns
// ============================================================

void movingDot() {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = CHSV(gHue, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  gHue++;
  showFrame(16);
}

void cometTail() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  static uint8_t pos = 0;
  leds[pos] = CHSV(gHue++, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  showFrame(18);
}

void sinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, gSat, 255);
  showFrame(14);
}

void chaseRainbow() {
  fadeToBlackBy(leds, NUM_LEDS, 64);
  int pos = beatsin16(10, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(gHue++, 255, 255);
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

void bounceComets() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[pos] = CHSV(millis() / 10, 255, 255);
  pos += dir;
  if (pos <= 0 || pos >= NUM_LEDS - 1) dir = -dir;
}

void waveGlide() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = sin8(i * 3 + millis() / 5);
    leds[i] = CHSV(gHue + index, gSat, 255);
  }
  showFrame(20);
}

void dotTrail() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  static uint8_t dotPos = 0;
  leds[dotPos] = CHSV(gHue++, 255, 255);
  dotPos = (dotPos + 1) % NUM_LEDS;
}

// ============================================================
// CATEGORY: Dynamic & Interactive Patterns
// ============================================================

void confetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), gSat, 255);
  showFrame(14);
}

void confettiPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[random16(NUM_LEDS)] += CHSV(gHue + random8(64), 200, 255);
  FastLED.show();
  delay(15);
}

void juggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(gHue + i * 32, gSat, 255);
  }
  showFrame(14);
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

void twinkle2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 50) {
    leds[random16(NUM_LEDS)] = CHSV(random8(), 200, 255);
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

void glitterFade() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 80) {
    leds[random16(NUM_LEDS)] += CHSV(random8(), 200, 255);
  }
}

void sparkle() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
  FastLED.show();
  delay(20);
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

// ============================================================
// CATEGORY: Breathing & Pulse Patterns
// ============================================================

void breathingEffect() {
  static uint8_t brightness = 0;
  static int delta = 5;
  fill_solid(leds, NUM_LEDS, CHSV(gHue, gSat, brightness));
  brightness += delta;
  if (brightness == 0 || brightness >= gBrightness) delta = -delta;
  showFrame(16);
}

void radiantWaves() {
  uint8_t wave = sin8(millis() / 3);
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, wave));
  FastLED.show();
  delay(20);
}

void centerPulse() {
  int center = NUM_LEDS / 2;
  uint8_t brightness = sin8(millis() / 5);
  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(i - center);
    leds[i] = CHSV(gHue, 255, brightness - dist * 8);
  }
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

void rainbowBeat() {
  uint8_t beatA = beatsin8(17, 0, 255);
  uint8_t beatB = beatsin8(13, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * beatA / 50), 255, beatB);
  }
  FastLED.show();
  delay(20);
}

void auraGlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t glow = sin8(millis() / 10 + i * 4);
    leds[i] = CHSV(gHue + i * 2, gSat, glow);
  }
  gHue++;
  showFrame(22);
}

// ============================================================
// CATEGORY: Wave & Ripple Patterns
// ============================================================

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

void smoothWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((sin8(i * 2 + millis() / 10)), 255, 200);
  }
}

void waveformShimmer() {
  static uint8_t offset = 0;
  offset += 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(180, 50, sin8(i * 6 + offset) / 2);
  }
}

// ============================================================
// CATEGORY: Fire & Heat Patterns
// ============================================================

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

void candleFlicker() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, random8(100, 255), random8(0, 50));
  }
  FastLED.show();
  delay(50);
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

void noiseLava() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 10, millis() / 5);
    leds[i] = CHSV(noise, 255, noise);
  }
}

// ============================================================
// CATEGORY: Electric & Neon Patterns
// ============================================================

void electricPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 45);
  uint8_t beat = beatsin8(24, 0, NUM_LEDS - 1);
  leds[beat] = CHSV(gHue + 160, gSat, 255);
  if (beat > 0) leds[beat - 1] = CHSV(gHue + 160, gSat, 180);
  if (beat < NUM_LEDS - 1) leds[beat + 1] = CHSV(gHue + 160, gSat, 180);
  showFrame(12);
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

// ============================================================
// CATEGORY: Nature & Weather Patterns
// ============================================================

void auroraBorealis() {
  static uint8_t baseHue = 90;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 20, millis() / 4);
    leds[i] = CHSV(gHue + baseHue + noise / 3, gSat, noise);
  }
  showFrame(22);
}

void auroraWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(96 + sin8(millis() / 20 + i * 2) / 8, 255, sin8(millis() / 10 + i * 3));
  }
}

void oceanCurrent() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8(i * 8 + millis() / 4);
    leds[i] = CHSV(gHue + 140 + wave / 12, gSat, wave);
  }
  showFrame(22);
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

void sunrise() {
  static uint8_t brightness = 0;
  fill_solid(leds, NUM_LEDS, CHSV(gHue + 10, gSat, brightness));
  brightness = qadd8(brightness, 1);
  if (brightness >= 255) brightness = 0;
  showFrame(26);
}

// ============================================================
// CATEGORY: Cosmic & Space Patterns
// ============================================================

void galaxySwirl() {
  static uint8_t centerHue = 180;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t angle = (i * 4 + millis() / 8) % 255;
    leds[i] = CHSV(gHue + centerHue + angle, gSat, sin8(angle));
  }
  centerHue += 1;
  showFrame(22);
}

void plasma() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, sin8(i * 8 + millis() / 3));
  }
  showFrame(20);
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

void mysticFlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 4 + millis() / 8) % 255, 200, 255);
  }
}

// ============================================================
// CATEGORY: Special & Festive Patterns
// ============================================================

void candyCaneTwist() {
  static uint8_t phase = 0;
  phase += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    bool stripe = ((i + phase) / 6) % 2 == 0;
    leds[i] = stripe ? CHSV(gHue, gSat, 255) : CHSV(gHue + 128, gSat, 255);
  }
  showFrame(24);
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

void shimmer() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 20) {
      leds[i] = CHSV(random8(), 200, random8(100, 255));
    } else {
      leds[i].fadeToBlackBy(20);
    }
  }
}

void shimmerCurtain() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160, 255, random8() > 240 ? 255 : 0);
  }
}

void pastelTwinkle() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  if (random8() < 60) {
    int pos = random(NUM_LEDS);
    leds[pos] = CHSV(random8(), 100, 255);
  }
}

void fallingStars() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  int pos = random(NUM_LEDS);
  leds[pos] = CHSV(0, 0, 255);
}

// ============================================================
// CATEGORY: Holiday & Seasonal Patterns
// ============================================================

void christmasSnowflakes() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  for (int i = 0; i < NUM_LEDS; i += 10) {
    leds[i] = CHSV(160, 200, 50);
  }
  if (random8() < 55) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CRGB::White;
    if (pos > 0) leds[pos - 1] = CHSV(160, 80, 140);
    if (pos < NUM_LEDS - 1) leds[pos + 1] = CHSV(160, 80, 140);
  }
  showFrame(28);
}

void christmasLights() {
  static uint8_t phase = 0;
  phase++;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = ((i / 4 + phase / 8) % 2) ? 0 : 96;
    uint8_t val = sin8(i * 20 + phase * 3) / 2 + 128;
    leds[i] = CHSV(hue, 255, val);
  }
  if (random8() < 30) {
    leds[random16(NUM_LEDS)] = CHSV(40, 200, 255);
  }
  showFrame(22);
}

void halloweenGhosts() {
  static int ghostPos = NUM_LEDS / 2;
  static int ghostDir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 35);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(192, 255, 35);
  }
  for (int g = -6; g <= 6; g++) {
    int idx = ghostPos + g;
    if (idx >= 0 && idx < NUM_LEDS) {
      leds[idx] = CHSV(192, 0, 255 - abs(g) * 35);
    }
  }
  ghostPos += ghostDir;
  if (ghostPos >= NUM_LEDS - 4 || ghostPos <= 4) {
    ghostDir = -ghostDir;
  }
  showFrame(18);
}

void halloweenPumpkin() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(24, 255, random8(160, 255));
  }
  if (random8() < 18) {
    leds[random16(NUM_LEDS)] = CHSV(192, 255, 200);
  }
  showFrame(32);
}

void easterPastelEggs() {
  static uint8_t shift = 0;
  shift += 2;
  const uint8_t pastelHues[] = {224, 160, 96, 64, 200};
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t band = (i / 8 + shift / 4) % 5;
    uint8_t val = sin8(i * 6 + shift) / 2 + 128;
    leds[i] = CHSV(pastelHues[band], 120, val);
  }
  showFrame(24);
}

void newYearFireworks() {
  fadeToBlackBy(leds, NUM_LEDS, 50);
  if (random8() < 28) {
    int center = random16(NUM_LEDS);
    uint8_t hue = random8();
    for (int d = -7; d <= 7; d++) {
      int idx = center + d;
      if (idx >= 0 && idx < NUM_LEDS) {
        leds[idx] = CHSV(hue, 255, 255 - abs(d) * 32);
      }
    }
  }
  showFrame(20);
}

void diwaliDiyas() {
  fadeToBlackBy(leds, NUM_LEDS, 35);
  for (int i = 0; i < NUM_LEDS; i += 14) {
    uint8_t flicker = inoise8(i * 50, millis() / 4) / 2 + 128;
    for (int j = 0; j < 4 && i + j < NUM_LEDS; j++) {
      leds[i + j] = CHSV(20, 255, flicker - j * 45);
    }
  }
  if (random8() < 45) {
    leds[random16(NUM_LEDS)] = CHSV(40, 255, 255);
  }
  showFrame(26);
}

void stPatricksShamrock() {
  static uint8_t wave = 0;
  wave += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(96, 255, sin8(i * 10 + wave));
  }
  if (random8() < 22) {
    leds[random16(NUM_LEDS)] = CHSV(64, 200, 255);
  }
  showFrame(20);
}

void valentinesHeartbeat() {
  static uint8_t beat = 80;
  static int8_t delta = 10;
  beat += delta;
  if (beat >= 230 || beat <= 25) {
    delta = -delta;
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(224, 180, beat);
  }
  showFrame(16);
}

void fourthOfJuly() {
  static uint8_t phase = 0;
  phase++;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t stripe = (i * 3 / NUM_LEDS + phase / 24) % 3;
    if (stripe == 0) {
      leds[i] = CHSV(0, 255, 255);
    } else if (stripe == 1) {
      leds[i] = CHSV(0, 0, 255);
    } else {
      leds[i] = CHSV(160, 255, 255);
    }
  }
  showFrame(25);
}

// ============================================================
// CATEGORY: Indian Festivals & Seasons
// ============================================================

void holiColorSplash() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  if (random8() < 45) {
    int center = random16(NUM_LEDS);
    uint8_t hue = random8();
    for (int d = -5; d <= 5; d++) {
      int idx = center + d;
      if (idx >= 0 && idx < NUM_LEDS) {
        leds[idx] = CHSV(hue, 255, 255 - abs(d) * 40);
      }
    }
  }
  for (int i = 0; i < NUM_LEDS; i += 6) {
    leds[i].fadeToBlackBy(10);
    leds[i] += CHSV((i * 17 + millis() / 20) % 255, 255, 30);
  }
  showFrame(18);
}

void navratriGarba() {
  static uint8_t spin = 0;
  spin += 5;
  const uint8_t navHues[] = {0, 24, 48, 72, 96, 128, 160, 192, 224};
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t colorIdx = (i / 6 + spin / 12) % 9;
    uint8_t val = sin8(i * 14 + spin * 2) / 2 + 140;
    leds[i] = CHSV(navHues[colorIdx], 255, val);
  }
  showFrame(16);
}

void rakhiCelebration() {
  static uint8_t phase = 0;
  phase += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    bool band = ((i + phase) / 5) % 3 == 0;
    if (band) {
      leds[i] = CHSV(0, 255, sin8(i * 12 + phase * 4) / 2 + 140);
    } else {
      leds[i] = CHSV(40, 220, 80);
    }
  }
  if (random8() < 35) {
    leds[random16(NUM_LEDS)] = CHSV(0, 0, 255);
  }
  showFrame(22);
}

void ganeshAarti() {
  static uint8_t wave = 0;
  wave += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t flame = sin8(i * 8 + wave) / 2 + 140;
    leds[i] = CHSV(24, 255, flame);
  }
  if (random8() < 25) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(48, 200, 255);
    if (pos > 0) leds[pos - 1] = CHSV(32, 255, 180);
  }
  showFrame(20);
}

void pongalHarvest() {
  static uint8_t dawn = 0;
  dawn++;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t gold = sin8(i * 6 + dawn) / 2 + 120;
    uint8_t green = sin8(i * 4 - dawn / 2) / 4 + 60;
    leds[i] = CHSV(40, 200, gold);
    leds[i] += CHSV(80, 180, green);
  }
  showFrame(24);
}

void monsoonRains() {
  fadeToBlackBy(leds, NUM_LEDS, 35);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(96, 180, 40);
  }
  for (int drop = 0; drop < 4; drop++) {
    if (random8() < 120) {
      int pos = random16(NUM_LEDS);
      leds[pos] = CHSV(160, 120, 220);
      if (pos > 0) leds[pos - 1] = CHSV(160, 80, 100);
    }
  }
  if (random8() < 12) {
    leds[random16(NUM_LEDS)] = CHSV(160, 60, 255);
  }
  showFrame(22);
}

void onamPookalam() {
  static uint8_t ring = 0;
  ring += 2;
  const uint8_t floralHues[] = {0, 24, 48, 96, 160, 192, 224};
  int center = NUM_LEDS / 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(i - center);
    uint8_t band = (dist / 4 + ring / 6) % 7;
    uint8_t val = sin8(dist * 12 + ring) / 2 + 130;
    leds[i] = CHSV(floralHues[band], 230, val);
  }
  showFrame(20);
}

void janmashtamiPeacock() {
  static uint8_t shimmer = 0;
  shimmer += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t base = sin8(i * 9 + shimmer);
    uint8_t hue = 128 + base / 8;
    uint8_t sat = 255 - base / 4;
    leds[i] = CHSV(hue, sat, base);
  }
  if (random8() < 28) {
    leds[random16(NUM_LEDS)] = CHSV(64, 255, 255);
  }
  showFrame(18);
}

void baisakhiFields() {
  static uint8_t breeze = 0;
  breeze += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wheat = sin8(i * 5 + breeze) / 2 + 100;
    leds[i] = CHSV(48, 220, wheat);
    if ((i + breeze / 4) % 9 < 2) {
      leds[i] += CHSV(96, 255, 80);
    }
  }
  showFrame(22);
}

void makarSankrantiKites() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160, 80, 60);
  }
  static int kitePos = 0;
  static int kiteDir = 1;
  static uint8_t kiteHue = 0;
  if (random8() < 8) {
    kiteHue = random8();
  }
  for (int k = -4; k <= 4; k++) {
    int idx = kitePos + k;
    if (idx >= 0 && idx < NUM_LEDS) {
      leds[idx] = CHSV(kiteHue, 255, 255 - abs(k) * 45);
    }
  }
  kitePos += kiteDir;
  if (kitePos >= NUM_LEDS - 5 || kitePos <= 5) {
    kiteDir = -kiteDir;
  }
  showFrame(16);
}

void durgaPujaDhak() {
  static uint8_t pulse = 0;
  static int8_t delta = 12;
  pulse += delta;
  if (pulse >= 240 || pulse <= 20) {
    delta = -delta;
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t band = (i / 7) % 2;
    uint8_t hue = band ? 0 : 224;
    leds[i] = CHSV(hue, 255, pulse);
  }
  if (random8() < 40) {
    leds[random16(NUM_LEDS)] = CHSV(40, 255, 255);
  }
  showFrame(14);
}

void summerMangoGlow() {
  static uint8_t ripen = 0;
  ripen++;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t mango = sin8(i * 7 + ripen) / 2 + 130;
    leds[i] = CHSV(32, 255, mango);
    if ((i + ripen / 3) % 11 == 0) {
      leds[i] += CHSV(64, 200, 90);
    }
  }
  showFrame(26);
}

// ============================================================
// NEW: Additional Unique Patterns
// ============================================================

void sineSpiral() {
  static uint16_t t = 0;
  t += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t angle = sin8(i * 3 + t / 2);
    uint8_t hue = (gHue + i * 6 + angle) & 0xFF;
    uint8_t val = sin8(i * 4 + t / 3);
    leds[i] = CHSV(hue, gSat, val);
  }
  gHue++;
  showFrame(18);
}

void colorTunnel() {
  static uint16_t offset = 0;
  offset += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t p = (i * 10 + offset) & 0xFF;
    leds[i] = CHSV(p, 200, 255 - (i % 20));
  }
  showFrame(14);
}

void meteorShower() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 64);
  int tail = 8;
  for (int i = 0; i < tail; i++) {
    int idx = pos - i * dir;
    if (idx >= 0 && idx < NUM_LEDS) {
      leds[idx] = CHSV(gHue + i * 4, 255, 255 - i * 28);
    }
  }
  pos += dir;
  if (pos >= NUM_LEDS || pos < 0) {
    dir = -dir;
    pos = constrain(pos, 0, NUM_LEDS - 1);
  }
  gHue++;
  showFrame(12);
}

// ============================================================
// OUT OF THE WORLD PATTERNS
// ============================================================

void quantumVortex() {
  static uint16_t t = 0;
  t += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t v = sin8(i * 7 + t) ^ cos8(i * 13 - t / 2);
    uint8_t h = (gHue + i * 5 + t / 3) & 0xFF;
    leds[i] = CHSV(h, 255, v);
  }
  gHue++;
  showFrame(14);
}

void alienAurora() {
  static uint16_t t = 0;
  t += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t n = inoise8(i * 18, t);
    leds[i] = CHSV(120 + n / 3, 255 - n, n);
  }
  showFrame(18);
}

void hypernovaBurst() {
  static uint16_t t = 0;
  t += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t burst = sin8(i * 8 + t) * cos8(i * 5 - t / 2);
    uint8_t h = (gHue + burst) & 0xFF;
    leds[i] = CHSV(h, 255, abs(burst));
  }
  gHue += 2;
  showFrame(12);
}

#endif

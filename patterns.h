#ifndef PATTERNS_H
#define PATTERNS_H

// =============================================================================
// patterns.h — All LED animation functions
// =============================================================================
//
// Each function in this file is one animation pattern. They are called once
// per loop() iteration by runCurrentPattern() in pattern_runner.ino.
//
// RULES FOR EVERY PATTERN FUNCTION
//   1. Write into the leds[] array.
//   2. End with showFrame(N) where N is the natural frame delay in milliseconds.
//      showFrame() pushes the frame to the strip, applies the speed setting,
//      and calls yield() so Wi-Fi stays responsive. See colormanagement.h.
//   3. Use "static" local variables to keep state between calls.
//      Static variables are initialised once and persist across frames.
//   4. Do NOT call delay() directly — use showFrame() instead.
//   5. Do NOT call FastLED.setBrightness() — it is set once when brightness
//      changes via the web UI.
//
// HOW TO ADD A NEW PATTERN
//   Copy the template below, fill it in, then register it in pattern_runner.ino.
//
// PATTERN TEMPLATE
// ─────────────────────────────────────────────────────────────────────────────
// // One sentence describing what the pattern looks like.
// void myPatternName() {
//   static uint8_t phase = 0;          // persistent state between frames
//
//   fadeToBlackBy(leds, NUM_LEDS, 20); // optional: fade existing pixels
//
//   for (int i = 0; i < NUM_LEDS; i++) {
//     leds[i] = CHSV(gHue + i * 5, gSat, 200); // write pixels
//   }
//
//   phase++;                           // advance animation state
//   showFrame(20);                     // display and wait (20 ms base delay)
// }
// ─────────────────────────────────────────────────────────────────────────────
//
// CHSV(hue, saturation, value) quick reference:
//   hue:        0=red  42=orange  85=green  128=cyan  170=blue  212=magenta
//   saturation: 0=white/grey  255=vivid color
//   value:      0=off  255=full brightness
//
// =============================================================================

// =============================================================================
// Rainbow & Color Cycling
// =============================================================================

// Full rainbow spread across the strip, slowly rotating through all hues.
// Full rainbow spread across the strip, slowly rotating through all hues.
void rainbowCycle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  gHue++;
  showFrame(20);
}

// Rainbow cycle with random white sparkle flashes scattered across the strip.
// Rainbow cycle with random white sparkle flashes scattered across the strip.
void rainbowGlitter() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * 7), gSat, 255);
  }
  if (random8() < 80) leds[random16(NUM_LEDS)] += CRGB::White;
  gHue++;
  showFrame(20);
}

// Rainbow that pulses in and out like a breathing effect.
// Rainbow that pulses in and out like a breathing effect.
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

// Slowly shifting rainbow with occasional bright white sparkles.
// Slowly shifting rainbow with occasional bright white sparkles.
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

// Entire strip cycles through solid colors one hue at a time.
// Entire strip cycles through solid colors one hue at a time.
void colorSweep() {
  static uint8_t sweepHue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(sweepHue, 255, 255);
  }
  sweepHue++;
  showFrame(30);
}

// Entire strip fades through solid colors, driven by gHue.
// Entire strip fades through solid colors, driven by gHue.
void colorFade() {
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
  gHue++;
  showFrame(20);
}

// Sine-wave color gradient that ripples along the strip over time.
// Sine-wave color gradient that ripples along the strip over time.
void colorWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 5), 255, 255);
  }
  showFrame(20);
}

// Smooth sine-wave gradient that glides along the strip, using gSat for richness.
// Smooth sine-wave gradient that glides along the strip, using gSat for richness.
void gradientWave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, 255);
  }
  showFrame(20);
}

// Scrolls through the active color palette (changed via the Palette selector).
// Scrolls through the active color palette (changed via the Palette selector).
void paletteCycle() {
  static uint8_t startIndex = 0;
  startIndex += 1;
  fill_palette(leds, NUM_LEDS, startIndex, 255 / NUM_LEDS, currentPalette, 255, LINEARBLEND);
  showFrame(18);
}

// =============================================================================
// Movement & Chase
// =============================================================================

// Single bright dot travels along the strip, leaving no trail.
// Single bright dot travels along the strip, leaving no trail.
void movingDot() {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[pos] = CHSV(gHue, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  gHue++;
  showFrame(16);
}

// Bright dot with a fading tail, like a comet streaking across the strip.
// Bright dot with a fading tail, like a comet streaking across the strip.
void cometTail() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  static uint8_t pos = 0;
  leds[pos] = CHSV(gHue++, gSat, 255);
  pos = (pos + 1) % NUM_LEDS;
  showFrame(18);
}

// Single dot bounces back and forth using a sine wave, leaving a fading trail.
// Single dot bounces back and forth using a sine wave, leaving a fading trail.
void sinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, gSat, 255);
  showFrame(14);
}

// Single dot bounces back and forth, cycling through rainbow colors.
// Single dot bounces back and forth, cycling through rainbow colors.
void chaseRainbow() {
  fadeToBlackBy(leds, NUM_LEDS, 64);
  int pos = beatsin16(10, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(gHue++, 255, 255);
  showFrame(20);
}

// Classic Cylon/KITT scanner: red dot bounces end to end with a fading trail.
// Classic Cylon/KITT scanner: red dot bounces end to end with a fading trail.
void cylonBounce() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[pos] = CRGB::Red;
  pos += dir;
  if (pos == NUM_LEDS - 1 || pos == 0) dir = -dir;
  showFrame(15);
}

// Dot bounces end to end, changing color continuously based on millis().
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Dot bounces end to end, changing color continuously based on millis().
// Note: uses millis() for timing — Speed slider has no effect.
void bounceComets() {
  static int pos = 0;
  static int dir = 1;
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[pos] = CHSV(millis() / 10, 255, 255);
  pos += dir;
  if (pos <= 0 || pos >= NUM_LEDS - 1) dir = -dir;
}

// Sine-wave color gradient that glides smoothly along the strip.
// Sine-wave color gradient that glides smoothly along the strip.
void waveGlide() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = sin8(i * 3 + millis() / 5);
    leds[i] = CHSV(gHue + index, gSat, 255);
  }
  showFrame(20);
}

// Dot travels the strip leaving a rainbow-colored fading trail.
// Note: no showFrame() call — runs at loop() speed.
// Dot travels the strip leaving a rainbow-colored fading trail.
// Note: no showFrame() call — runs at loop() speed.
void dotTrail() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  static uint8_t dotPos = 0;
  leds[dotPos] = CHSV(gHue++, 255, 255);
  dotPos = (dotPos + 1) % NUM_LEDS;
}

// =============================================================================
// Dynamic & Interactive
// =============================================================================

// Random colored pixels pop up and slowly fade, like confetti falling.
// Random colored pixels pop up and slowly fade, like confetti falling.
void confetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), gSat, 255);
  showFrame(14);
}

// Faster confetti: random pixels flash brightly and fade quickly.
// Faster confetti: random pixels flash brightly and fade quickly.
void confettiPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  leds[random16(NUM_LEDS)] += CHSV(gHue + random8(64), 200, 255);
  showFrame(15);
}

// Eight dots bounce at different speeds, each a different hue.
// Eight dots bounce at different speeds, each a different hue.
void juggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(gHue + i * 32, gSat, 255);
  }
  showFrame(14);
}

// Random pixels twinkle on and off in colors near gHue.
// Random pixels twinkle on and off in colors near gHue.
void twinkle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 20)
      leds[i] = CHSV(gHue + random8(96), gSat, 255);
    else
      leds[i].fadeToBlackBy(20);
  }
  showFrame(24);
}

// Random pixels flash in fully random colors and fade out.
// Random pixels flash in fully random colors and fade out.
void twinkle2() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 50) {
    leds[random16(NUM_LEDS)] = CHSV(random8(), 200, 255);
  }
  showFrame(20);
}

// Five random pixels flash in random colors each frame.
// Five random pixels flash in random colors each frame.
void dazzle() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  for (int i = 0; i < 5; i++) {
    leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
  }
  showFrame(30);
}

// Random pixels appear and fade, creating a soft glittering effect.
// Note: no showFrame() call — runs at loop() speed.
// Random pixels appear and fade, creating a soft glittering effect.
// Note: no showFrame() call — runs at loop() speed.
void glitterFade() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 80) {
    leds[random16(NUM_LEDS)] += CHSV(random8(), 200, 255);
  }
}

// Single random pixel flashes a random color against a black background.
// Single random pixel flashes a random color against a black background.
void sparkle() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
  showFrame(20);
}

// Random pixels pop to a random color then fade, like popping bubbles.
// Note: no showFrame() call — runs at loop() speed.
// Random pixels pop to a random color then fade, like popping bubbles.
// Note: no showFrame() call — runs at loop() speed.
void pixelPop() {
  if (random8() < 40) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(), 255, 255);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(15);
  }
}

// =============================================================================
// Breathing & Pulse
// =============================================================================

// Entire strip breathes in and out in a single color (gHue).
// Entire strip breathes in and out in a single color (gHue).
void breathingEffect() {
  static uint8_t brightness = 0;
  static int delta = 5;
  fill_solid(leds, NUM_LEDS, CHSV(gHue, gSat, brightness));
  brightness += delta;
  if (brightness == 0 || brightness >= gBrightness) delta = -delta;
  showFrame(16);
}

// Entire strip pulses in brightness using a sine wave driven by millis().
// Entire strip pulses in brightness using a sine wave driven by millis().
void radiantWaves() {
  uint8_t wave = sin8(millis() / 3);
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, wave));
  showFrame(20);
}

// Brightness pulses outward from the center, dimming toward the ends.
// Note: no showFrame() call — runs at loop() speed.
// Brightness pulses outward from the center, dimming toward the ends.
// Note: no showFrame() call — runs at loop() speed.
void centerPulse() {
  int center = NUM_LEDS / 2;
  uint8_t brightness = sin8(millis() / 5);
  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(i - center);
    leds[i] = CHSV(gHue, 255, brightness - dist * 8);
  }
}

// Beat-driven wave: brightness and hue ripple along the strip in sync with a beat.
// Beat-driven wave: brightness and hue ripple along the strip in sync with a beat.
void beatWave() {
  CRGBPalette16 palette = PartyColors_p;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t idx = beatsin8(6, 0, 255) + (i * 10);
    uint8_t val = sin8(idx);
    leds[i] = CHSV(gHue + idx, gSat, val);
  }
  showFrame(18);
}

// Rainbow hue shifts driven by two beat oscillators for a pulsing color effect.
// Rainbow hue shifts driven by two beat oscillators for a pulsing color effect.
void rainbowBeat() {
  uint8_t beatA = beatsin8(17, 0, 255);
  uint8_t beatB = beatsin8(13, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + (i * beatA / 50), 255, beatB);
  }
  showFrame(20);
}

// Sine-wave glow ripples along the strip, slowly shifting hue.
// Sine-wave glow ripples along the strip, slowly shifting hue.
void auraGlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t glow = sin8(millis() / 10 + i * 4);
    leds[i] = CHSV(gHue + i * 2, gSat, glow);
  }
  gHue++;
  showFrame(22);
}

// =============================================================================
// Wave & Ripple
// =============================================================================

// White ripple expands from a random center point, then resets.
// White ripple expands from a random center point, then resets.
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

// Colored ripple expands symmetrically from a random center, fading as it spreads.
// Note: no showFrame() call — runs at loop() speed.
// Colored ripple expands symmetrically from a random center, fading as it spreads.
// Note: no showFrame() call — runs at loop() speed.
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

// Smooth sine-wave color gradient scrolling along the strip using millis().
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Smooth sine-wave color gradient scrolling along the strip using millis().
// Note: uses millis() for timing — Speed slider has no effect.
void smoothWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((sin8(i * 2 + millis() / 10)), 255, 200);
  }
}

// Dim blue sine wave shimmers along the strip like light on water.
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Dim blue sine wave shimmers along the strip like light on water.
// Note: uses millis() for timing — Speed slider has no effect.
void waveformShimmer() {
  static uint8_t offset = 0;
  offset += 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(180, 50, sin8(i * 6 + offset) / 2);
  }
}

// =============================================================================
// Fire & Heat
// =============================================================================

// Classic fire simulation: heat rises from the base and cools toward the top.
// Classic fire simulation: heat rises from the base and cools toward the top.
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

// Warm flickering candle light: random orange-yellow variations across all pixels.
// Warm flickering candle light: random orange-yellow variations across all pixels.
void candleFlicker() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, random8(100, 255), random8(0, 50));
  }
  showFrame(50);
}

// Slow-moving lava: Perlin noise drives orange-red blobs drifting along the strip.
// Slow-moving lava: Perlin noise drives orange-red blobs drifting along the strip.
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

// Noise-driven lava: Perlin noise maps directly to hue and brightness.
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Noise-driven lava: Perlin noise maps directly to hue and brightness.
// Note: uses millis() for timing — Speed slider has no effect.
void noiseLava() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 10, millis() / 5);
    leds[i] = CHSV(noise, 255, noise);
  }
}

// =============================================================================
// Electric & Neon
// =============================================================================

// Electric blue-purple pulse bounces back and forth with a soft glow on each side.
// Electric blue-purple pulse bounces back and forth with a soft glow on each side.
void electricPulse() {
  fadeToBlackBy(leds, NUM_LEDS, 45);
  uint8_t beat = beatsin8(24, 0, NUM_LEDS - 1);
  leds[beat] = CHSV(gHue + 160, gSat, 255);
  if (beat > 0) leds[beat - 1] = CHSV(gHue + 160, gSat, 180);
  if (beat < NUM_LEDS - 1) leds[beat + 1] = CHSV(gHue + 160, gSat, 180);
  showFrame(12);
}

// Random neon pixels appear and smear forward, like streaks of light.
// Note: no showFrame() call — runs at loop() speed.
// Random neon pixels appear and smear forward, like streaks of light.
// Note: no showFrame() call — runs at loop() speed.
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

// Zooming color tunnel: hue and brightness ripple outward like a hyperspace jump.
// Zooming color tunnel: hue and brightness ripple outward like a hyperspace jump.
void hyperspaceTunnel() {
  static uint8_t zoom = 0;
  zoom += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((gHue + zoom + i * 8) % 255, gSat, sin8(i * 4 + zoom));
  }
  showFrame(16);
}

// =============================================================================
// Nature & Weather
// =============================================================================

// Green-teal aurora: Perlin noise drives shifting curtains of light.
// Green-teal aurora: Perlin noise drives shifting curtains of light.
void auroraBorealis() {
  static uint8_t baseHue = 90;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 20, millis() / 4);
    leds[i] = CHSV(gHue + baseHue + noise / 3, gSat, noise);
  }
  showFrame(22);
}

// Gentle aurora: sine waves modulate hue and brightness in cool blue-green tones.
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Gentle aurora: sine waves modulate hue and brightness in cool blue-green tones.
// Note: uses millis() for timing — Speed slider has no effect.
void auroraWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(96 + sin8(millis() / 20 + i * 2) / 8, 255, sin8(millis() / 10 + i * 3));
  }
}

// Deep blue ocean current: sine wave drives brightness like light through water.
// Deep blue ocean current: sine wave drives brightness like light through water.
void oceanCurrent() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8(i * 8 + millis() / 4);
    leds[i] = CHSV(gHue + 140 + wave / 12, gSat, wave);
  }
  showFrame(22);
}

// Dark storm with slow brightness pulses and occasional white lightning strikes.
// Dark storm with slow brightness pulses and occasional white lightning strikes.
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

// Slowly brightening warm glow that resets and repeats, like a sunrise.
// Slowly brightening warm glow that resets and repeats, like a sunrise.
void sunrise() {
  static uint8_t brightness = 0;
  fill_solid(leds, NUM_LEDS, CHSV(gHue + 10, gSat, brightness));
  brightness = qadd8(brightness, 1);
  if (brightness >= 255) brightness = 0;
  showFrame(26);
}

// =============================================================================
// Cosmic & Space
// =============================================================================

// Rotating galaxy: hue and brightness spiral outward from a shifting center.
// Rotating galaxy: hue and brightness spiral outward from a shifting center.
// Rotating galaxy: hue and brightness spiral outward from a shifting center.
void galaxySwirl() {
  static uint8_t centerHue = 180;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t angle = (i * 4 + millis() / 8) % 255;
    leds[i] = CHSV(gHue + centerHue + angle, gSat, sin8(angle));
  }
  centerHue += 1;
  showFrame(22);
}

// Plasma effect: two sine waves modulate hue and brightness independently.
// Plasma effect: two sine waves modulate hue and brightness independently.
// Plasma effect: two sine waves modulate hue and brightness independently.
void plasma() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue + sin8(i * 8 + millis() / 4), gSat, sin8(i * 8 + millis() / 3));
  }
  showFrame(20);
}

// Perlin noise drives hue across the strip, creating organic color clouds.
// Perlin noise drives hue across the strip, creating organic color clouds.
// Perlin noise drives hue across the strip, creating organic color clouds.
void noiseRainbow() {
  static uint16_t x = 0, y = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(x + i * 50, y);
    leds[i] = CHSV(gHue + noise, gSat, 255);
  }
  y += 20;
  showFrame(24);
}

// Perlin noise maps directly to hue, producing slowly morphing color gradients.
// Perlin noise maps directly to hue, producing slowly morphing color gradients.
// Perlin noise maps directly to hue, producing slowly morphing color gradients.
void perlinNoiseColors() {
  static uint16_t x = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(i * 30, x);
    leds[i] = CHSV(noise, 255, 255);
  }
  x += 5;
  showFrame(20);
}

// Perlin noise maps to hue with a desaturated palette, creating a soft gradient.
// Perlin noise maps to hue with a desaturated palette, creating a soft gradient.
// Perlin noise maps to hue with a desaturated palette, creating a soft gradient.
void noiseGradient() {
  static uint16_t noiseX = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(noiseX + i * 10, millis() / 5);
    leds[i] = CHSV(noise, 200, 255);
  }
  noiseX += 3;
  showFrame(30);
}

// Smooth hue gradient scrolls along the strip continuously using millis().
// Note: uses millis() for timing instead of showFrame(), so Speed has no effect.
// Smooth hue gradient scrolls along the strip continuously using millis().
// Note: uses millis() for timing — Speed slider has no effect.
// Smooth hue gradient scrolls along the strip continuously using millis().
// Note: uses millis() for timing — Speed slider has no effect.
void mysticFlow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 4 + millis() / 8) % 255, 200, 255);
  }
}

// =============================================================================
// Special & Festive
// =============================================================================

// Two-color alternating stripes scroll along the strip (complementary hues).
// Two-color alternating stripes scroll along the strip (complementary hues).
void candyCaneTwist() {
  static uint8_t phase = 0;
  phase += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    bool stripe = ((i + phase) / 6) % 2 == 0;
    leds[i] = stripe ? CHSV(gHue, gSat, 255) : CHSV(gHue + 128, gSat, 255);
  }
  showFrame(24);
}

// Sine wave modulates both hue and brightness, creating a spinning vortex look.
// Sine wave modulates both hue and brightness, creating a spinning vortex look.
void vortexSpin() {
  static uint8_t spin = 0;
  spin += 3;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8((i * 11) + spin);
    leds[i] = CHSV(gHue + wave, gSat, wave);
  }
  showFrame(16);
}

// Green digital rain: random bright pixels fall in columns like the Matrix.
// Green digital rain: random bright pixels fall in columns like the Matrix.
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

// Random pixels flash in random colors and fade, creating a shimmering texture.
// Note: no showFrame() call — runs at loop() speed.
// Random pixels flash in random colors and fade, creating a shimmering texture.
// Note: no showFrame() call — runs at loop() speed.
void shimmer() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 20) {
      leds[i] = CHSV(random8(), 200, random8(100, 255));
    } else {
      leds[i].fadeToBlackBy(20);
    }
  }
}

// Blue curtain: pixels flash fully on or fully off at random, like a bead curtain.
// Note: no showFrame() call — runs at loop() speed.
// Blue curtain: pixels flash fully on or fully off at random, like a bead curtain.
// Note: no showFrame() call — runs at loop() speed.
void shimmerCurtain() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160, 255, random8() > 240 ? 255 : 0);
  }
}

// Soft pastel pixels appear randomly and fade, like gentle twinkling lights.
// Note: no showFrame() call — runs at loop() speed.
// Soft pastel pixels appear randomly and fade, like gentle twinkling lights.
// Note: no showFrame() call — runs at loop() speed.
void pastelTwinkle() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  if (random8() < 60) {
    int pos = random(NUM_LEDS);
    leds[pos] = CHSV(random8(), 100, 255);
  }
}

// Single white pixel appears at a random position each frame, like a falling star.
// Note: no showFrame() call — runs at loop() speed.
// Single white pixel appears at a random position each frame, like a falling star.
// Note: no showFrame() call — runs at loop() speed.
void fallingStars() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  int pos = random(NUM_LEDS);
  leds[pos] = CHSV(0, 0, 255);
}

// =============================================================================
// Global Holidays & Seasonal
// =============================================================================

// Christmas snowflakes: cool blue base with bright white snowflake flashes.
// Christmas snowflakes: cool blue base with bright white snowflake flashes.
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

// Christmas lights: alternating red and green segments with warm golden sparkles.
// Christmas lights: alternating red and green segments with warm golden sparkles.
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

// Halloween ghosts: purple background with a white ghost shape bouncing end to end.
// Halloween ghosts: purple background with a white ghost shape bouncing end to end.
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

// Halloween pumpkin: flickering orange glow with occasional purple flashes.
// Halloween pumpkin: flickering orange glow with occasional purple flashes.
void halloweenPumpkin() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(24, 255, random8(160, 255));
  }
  if (random8() < 18) {
    leds[random16(NUM_LEDS)] = CHSV(192, 255, 200);
  }
  showFrame(32);
}

// Easter pastel eggs: five soft pastel color bands scroll slowly along the strip.
// Easter pastel eggs: five soft pastel color bands scroll slowly along the strip.
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

// New Year fireworks: random bursts of color explode symmetrically along the strip.
// New Year fireworks: random bursts of color explode symmetrically along the strip.
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

// Diwali diyas: evenly spaced flickering oil lamps with golden spark accents.
// Diwali diyas: evenly spaced flickering oil lamps with golden spark accents.
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

// St. Patrick's Day: bright green sine wave with occasional yellow-green sparkles.
// St. Patrick's Day: bright green sine wave with occasional yellow-green sparkles.
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

// Valentine's Day: entire strip pulses in deep red like a heartbeat.
// Valentine's Day: entire strip pulses in deep red like a heartbeat.
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

// Fourth of July: red, white, and blue stripes scroll along the strip.
// Fourth of July: red, white, and blue stripes scroll along the strip.
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

// =============================================================================
// Indian Festivals & Seasons
// =============================================================================

// Holi: random bursts of vivid color splash across the strip.
// Holi: random bursts of vivid color splash across the strip.
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

// Navratri Garba: nine festival colors spin rapidly along the strip.
// Navratri Garba: nine festival colors spin rapidly along the strip.
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

// Rakhi: red and gold alternating bands with occasional white sparkles.
// Rakhi: red and gold alternating bands with occasional white sparkles.
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

// Ganesh Aarti: warm orange flame wave with golden spark accents.
// Ganesh Aarti: warm orange flame wave with golden spark accents.
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

// Pongal harvest: golden and green sine waves blend like sunlit fields.
// Pongal harvest: golden and green sine waves blend like sunlit fields.
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

// Monsoon rains: green base with blue raindrops falling at random positions.
// Monsoon rains: green base with blue raindrops falling at random positions.
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

// Onam Pookalam: seven floral colors radiate outward from the center like a rangoli.
// Onam Pookalam: seven floral colors radiate outward from the center like a rangoli.
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

// Janmashtami Peacock: iridescent blue-green shimmer with golden feather flashes.
// Janmashtami Peacock: iridescent blue-green shimmer with golden feather flashes.
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

// Baisakhi fields: golden wheat waves with patches of green, like a harvest field.
// Baisakhi fields: golden wheat waves with patches of green, like a harvest field.
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

// Makar Sankranti kites: sky-blue background with a colorful kite bouncing end to end.
// Makar Sankranti kites: sky-blue background with a colorful kite bouncing end to end.
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

// Durga Puja Dhak: red and purple alternating bands pulse like a drumbeat.
// Durga Puja Dhak: red and purple alternating bands pulse like a drumbeat.
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

// Summer mango glow: warm orange sine wave with occasional green highlights.
// Summer mango glow: warm orange sine wave with occasional green highlights.
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

// =============================================================================
// Additional Holiday Variants
// =============================================================================

// Diwali fireworks: random bursts of colored sparks shooting to one side.
void diwaliFireworks() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  if (random8() < 30) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(random8(), 255, 255);
    for (int j = 1; j < 6; j++) {
      int idx = pos + j;
      if (idx < NUM_LEDS) leds[idx] = CHSV(random8(), 200, 180 - j * 30);
    }
  }
  showFrame(18);
}

// Easter eggs: repeating bands of soft pastel colors with occasional white flashes.
void easterEggs() {
  static const CRGB pastelColors[] = {CRGB(255, 182, 193), CRGB(176, 224, 230), CRGB(152, 251, 152), CRGB(255, 239, 213), CRGB(221, 160, 221)};
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pastelColors[(i / 8) % 5];
  }
  if (random8() < 10) leds[random16(NUM_LEDS)] = CRGB::White;
  showFrame(26);
}

// New Year bubbles: sparse warm-white dots rising like champagne bubbles.
void newYearBubbles() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  for (int i = 0; i < 3; i++) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(40, 30, 255);
  }
  showFrame(24);
}

// =============================================================================
// Geometric & Mathematical
// =============================================================================

// Sine spiral: two overlapping sine waves modulate hue and brightness, creating a spiraling color tunnel.
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

// Color tunnel: hue scrolls along the strip like flying through a colored tube.
void colorTunnel() {
  static uint16_t offset = 0;
  offset += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t p = (i * 10 + offset) & 0xFF;
    leds[i] = CHSV(p, 200, 255 - (i % 20));
  }
  showFrame(14);
}

// Meteor shower: a comet with a fading tail bounces back and forth, shifting hue.
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

// =============================================================================
// Experimental
// =============================================================================

// Quantum vortex: XOR of two sine waves at different frequencies produces chaotic, shifting color bursts.
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

// Alien aurora: Perlin noise drives a cool green-teal aurora with organic movement.
void alienAurora() {
  static uint16_t t = 0;
  t += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t n = inoise8(i * 18, t);
    leds[i] = CHSV(120 + n / 3, 255 - n, n);
  }
  showFrame(18);
}

// Hypernova burst: multiplied sine/cosine waves create explosive, chaotic color flares.
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

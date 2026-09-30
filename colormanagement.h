#ifndef COLOR_MANAGEMENT_H
#define COLOR_MANAGEMENT_H

// ── Global controller state ───────────────────────────────────────────────────
// Defined in fastLED.ino; declared here so all .ino/.h files can use them.
extern uint8_t gHue;
extern uint8_t gSat;
extern uint8_t gBrightness;
extern uint8_t gSpeed;

// ── Color-picker override ─────────────────────────────────────────────────────
extern uint8_t colorPickerR;
extern uint8_t colorPickerG;
extern uint8_t colorPickerB;
extern bool    useColorPickerOverride;

// ── Palette ───────────────────────────────────────────────────────────────────
extern CRGBPalette16 currentPalette;

// ── LED array (defined in fastLED.ino) ───────────────────────────────────────
extern CRGB leds[];

// ── Helpers ───────────────────────────────────────────────────────────────────

inline void setColorPickerColor(uint8_t r, uint8_t g, uint8_t b) {
  colorPickerR = r;
  colorPickerG = g;
  colorPickerB = b;
  useColorPickerOverride = true;
}

inline CRGB getColorPickerRGB() {
  return CRGB(colorPickerR, colorPickerG, colorPickerB);
}

// Returns a delay in ms scaled by gSpeed (1=slowest, 100=fastest).
inline uint16_t scaledDelay(uint16_t baseDelay) {
  uint8_t spd = constrain(gSpeed, 1, 100);
  uint16_t minD = max((uint16_t)1, (uint16_t)(baseDelay / 4));
  uint16_t maxD = baseDelay * 3;
  return map(spd, 1, 100, maxD, minD);
}

// Non-blocking frame display: shows LEDs and yields for the scaled delay.
// Does NOT call setBrightness every frame — caller must do that when it changes.
inline void showFrame(uint16_t baseDelay) {
  FastLED.show();
  FastLED.delay(scaledDelay(baseDelay));
  yield();
}

#endif // COLOR_MANAGEMENT_H

// =============================================================================
// colormanagement.h — Shared LED state and frame-rendering helpers
// =============================================================================
//
// This file does two things:
//
//   1. Declares the global variables that hold the current LED state
//      (brightness, hue, speed, etc.) so that patterns.h, web_ui.ino, and
//      pattern_runner.ino can all read and write the same values.
//
//   2. Provides small helper functions used by every pattern.
//
// WHY "extern"?
//   Arduino compiles each .ino file as a separate translation unit.
//   The variables are *defined* (memory allocated) once in fastLED.ino.
//   The "extern" keyword here tells the compiler "this variable exists
//   somewhere else — just let me use it." Without extern, each file that
//   includes this header would create its own copy, causing linker errors.
//
// =============================================================================

#ifndef COLOR_MANAGEMENT_H
#define COLOR_MANAGEMENT_H

// -----------------------------------------------------------------------------
// Animation controls
//
// These are changed by the web UI and saved to flash between reboots.
// Patterns read these values every frame to produce their visual output.
//
// gHue         0–255  Color wheel position. 0=red, 85=green, 170=blue.
//                     Many patterns slowly increment this each frame to
//                     produce a slowly shifting color over time.
//
// gSat         0–255  Color saturation. 255=vivid color, 0=white/grey.
//
// gBrightness  0–255  Overall LED brightness. Applied via FastLED.setBrightness().
//                     Changing this does NOT require updating every LED pixel —
//                     FastLED scales the output automatically.
//
// gSpeed       1–100  Animation speed. Higher = faster.
//                     Passed to showFrame() which converts it to a frame delay.
// -----------------------------------------------------------------------------
extern uint8_t gHue;
extern uint8_t gSat;
extern uint8_t gBrightness;
extern uint8_t gSpeed;

// -----------------------------------------------------------------------------
// Color-picker override
//
// When the user picks a specific RGB color in the web UI and clicks
// "Apply color", useColorPickerOverride is set to true and the chosen
// color is stored in colorPickerR/G/B.
//
// Patterns that want to respect the color override can call getColorPickerRGB()
// instead of computing their own color. Most patterns ignore this and use
// gHue/gSat instead — the override is applied by fill_solid() in the route
// handler, which overwrites the LED buffer directly.
// -----------------------------------------------------------------------------
extern uint8_t colorPickerR;
extern uint8_t colorPickerG;
extern uint8_t colorPickerB;
extern bool    useColorPickerOverride;

// -----------------------------------------------------------------------------
// Active color palette
//
// Used by patterns that call fill_palette() or ColorFromPalette().
// Changed via the /palette API endpoint or the Palette selector in the UI.
// Defaults to RainbowColors_p on boot.
// Only patterns with the PAT_USES_PAL flag actually read this.
// -----------------------------------------------------------------------------
extern CRGBPalette16 currentPalette;

// -----------------------------------------------------------------------------
// LED pixel buffer
//
// The array of RGB values that FastLED writes to the physical strip.
// Defined in fastLED.ino as:  CRGB leds[NUM_LEDS];
// Every pattern writes into this array, then calls showFrame() to push
// the values to the hardware.
// -----------------------------------------------------------------------------
extern CRGB leds[];

// -----------------------------------------------------------------------------
// setColorPickerColor(r, g, b)
//
// Called by the /setColor web route when the user applies a color override.
// Stores the chosen color and sets the override flag so the UI can reflect
// the current state correctly.
// -----------------------------------------------------------------------------
inline void setColorPickerColor(uint8_t r, uint8_t g, uint8_t b) {
  colorPickerR = r;
  colorPickerG = g;
  colorPickerB = b;
  useColorPickerOverride = true;
}

// -----------------------------------------------------------------------------
// getColorPickerRGB()
//
// Returns the currently selected override color as a CRGB value.
// Useful for patterns that want to blend the user's chosen color into
// their animation rather than ignoring it entirely.
// -----------------------------------------------------------------------------
inline CRGB getColorPickerRGB() {
  return CRGB(colorPickerR, colorPickerG, colorPickerB);
}

// -----------------------------------------------------------------------------
// scaledDelay(baseDelay)
//
// Converts a pattern's natural frame delay into a speed-adjusted delay.
//
// Each pattern is written with a "natural" base delay in milliseconds —
// the delay that makes the animation look right at medium speed.
// This function maps gSpeed (1–100) onto a range around that base delay:
//
//   gSpeed=1   → baseDelay × 3   (very slow)
//   gSpeed=50  → baseDelay       (natural speed)
//   gSpeed=100 → baseDelay ÷ 4   (very fast, minimum 1 ms)
//
// Example: a pattern with baseDelay=20 ms runs at:
//   speed 1   → 60 ms per frame  (~16 fps)
//   speed 50  → 20 ms per frame  (~50 fps)
//   speed 100 →  5 ms per frame  (~200 fps)
// -----------------------------------------------------------------------------
inline uint16_t scaledDelay(uint16_t baseDelay) {
  uint8_t  spd  = constrain(gSpeed, 1, 100);
  uint16_t minD = max((uint16_t)1, (uint16_t)(baseDelay / 4));
  uint16_t maxD = baseDelay * 3;
  return map(spd, 1, 100, maxD, minD);
}

// -----------------------------------------------------------------------------
// showFrame(baseDelay)
//
// Every pattern should end with this call instead of FastLED.show() + delay().
//
// It does three things:
//   1. Pushes the current leds[] buffer to the physical strip.
//   2. Waits for the speed-adjusted delay (via scaledDelay above).
//   3. Calls yield() so the ESP8266 Wi-Fi stack gets CPU time.
//      Without yield(), the watchdog timer resets the device during
//      long-running patterns.
//
// Note: brightness is NOT set here. It is set once in setup() and again
// whenever the user changes it via the web UI. Setting it every frame
// would be wasteful.
// -----------------------------------------------------------------------------
inline void showFrame(uint16_t baseDelay) {
  FastLED.show();
  FastLED.delay(scaledDelay(baseDelay));
  yield();
}

#endif // COLOR_MANAGEMENT_H

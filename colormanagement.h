#ifndef COLOR_MANAGEMENT_H
#define COLOR_MANAGEMENT_H

// Global color state
uint8_t gHue = 0;
uint8_t gSat = 255;
uint8_t gBrightness = BRIGHTNESS;
uint8_t gSpeed = 20;

// Color Picker override state
uint8_t colorPickerR = 255;
uint8_t colorPickerG = 0;
uint8_t colorPickerB = 0;
bool useColorPickerOverride = false;

// Palette
CRGBPalette16 currentPalette = RainbowColors_p;

// Helper function to convert RGB to HSV for consistent color handling
void setColorPickerColor(uint8_t r, uint8_t g, uint8_t b) {
  colorPickerR = r;
  colorPickerG = g;
  colorPickerB = b;
  useColorPickerOverride = true;
  Serial.printf("Color picker set: R=%d, G=%d, B=%d\n", r, g, b);
}

// Get CRGB color from color picker
CRGB getColorPickerRGB() {
  return CRGB(colorPickerR, colorPickerG, colorPickerB);
}

// Scale delay based on speed setting
uint16_t scaledDelay(uint16_t baseDelay) {
  uint8_t speed = constrain(gSpeed, 1, 100);
  uint16_t minDelay = baseDelay / 4;
  if (minDelay < 1) minDelay = 1;
  uint16_t maxDelay = baseDelay * 3;
  return map(speed, 1, 100, maxDelay, minDelay);
}

// Display frame with brightness and delay
void showFrame(uint16_t baseDelay) {
  FastLED.setBrightness(gBrightness);
  FastLED.show();
  delay(scaledDelay(baseDelay));
}

#endif

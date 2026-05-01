#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LED_PIN D2
#define NUM_LEDS 168
#define BRIGHTNESS 128
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

const unsigned long WIFI_CONNECT_TIMEOUT_MS = 10000;
const unsigned long AUTO_CYCLE_INTERVAL_MS = 12750;

const char *patternNames[] = {
  // Rainbow & Color Cycling
  "Rainbow Cycle",
  "Rainbow Glitter",
  "Rainbow Pulse",
  "Rainbow Sparkle",
  "Color Sweep",
  "Color Fade",
  "Color Waves",
  "Gradient Wave",
  "Palette Cycle",
  
  // Movement & Chase
  "Moving Dot",
  "Comet Tail",
  "Sinelon",
  "Chase Rainbow",
  "Cylon Bounce",
  "Bounce Comets",
  "Wave Glide",
  "Dot Trail",
  
  // Dynamic & Interactive
  "Confetti",
  "Confetti Pulse",
  "Juggle",
  "Twinkle",
  "Twinkle Fade",
  "Dazzle",
  "Glitter Fade",
  "Sparkle",
  "Pixel Pop",
  
  // Breathing & Pulse
  "Breathing Effect",
  "Radiant Waves",
  "Center Pulse",
  "Beat Wave",
  "Rainbow Beat",
  "Aura Glow",
  
  // Wave & Ripple
  "Ripple",
  "Ripple Stars",
  "Smooth Waves",
  "Waveform Shimmer",
  
  // Fire & Heat
  "Fire Effect",
  "Candle Flicker",
  "Lava Flow",
  "Noise Lava",
  
  // Electric & Neon
  "Electric Pulse",
  "Neon Streaks",
  "Hyperspace Tunnel",
  
  // Nature & Weather
  "Aurora Borealis",
  "Aurora Waves",
  "Ocean Current",
  "Storm Pulse",
  "Sunrise",
  
  // Cosmic & Space
  "Galaxy Swirl",
  "Plasma",
  "Noise Rainbow",
  "Perlin Noise",
  "Noise Gradient",
  "Mystic Flow",
  
  // Special & Festive
  "Candy Cane Twist",
  "Vortex Spin",
  "Matrix Rain",
  "Shimmer",
  "Shimmer Curtain",
  "Pastel Twinkle",
  "Falling Stars"
};

const int TOTAL_PATTERNS = sizeof(patternNames) / sizeof(patternNames[0]);

#endif

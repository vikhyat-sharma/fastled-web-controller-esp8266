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

#endif

// =============================================================================
// pattern_runner.ino — Pattern registry and dispatcher
// =============================================================================
//
// This file has two jobs:
//
//   1. REGISTRY — a table (kPatterns[]) that lists every pattern by name,
//      links it to its function in patterns.h, and records which controls
//      affect it (the "flags").
//
//   2. DISPATCHER — runCurrentPattern(), called every loop() iteration,
//      which looks up the active pattern in the table and calls its function.
//
// ─────────────────────────────────────────────────────────────────────────────
// HOW TO ADD A NEW PATTERN
// ─────────────────────────────────────────────────────────────────────────────
//
//   Step 1 — Write the pattern function in patterns.h.
//             Follow the template at the top of that file.
//
//   Step 2 — Add one line to kPatterns[] below (always at the END of the list):
//
//               { "My Pattern Name",  myPatternFunction,  PAT_USES_HUE | PAT_USES_SPEED },
//
//             The index is assigned automatically by position in the array.
//             Never reorder or remove existing entries — the web UI and any
//             saved settings refer to patterns by their index number.
//
//   Step 3 — Set the flags honestly (see the flag reference below).
//             If you're unsure, use 0 and update it later.
//
//   That's it. TOTAL_PATTERNS updates automatically.
//
// ─────────────────────────────────────────────────────────────────────────────
// FLAG REFERENCE
// ─────────────────────────────────────────────────────────────────────────────
//
//   PAT_USES_HUE   — the pattern reads gHue.
//                    The Hue slider in the UI will visibly change this pattern.
//
//   PAT_USES_SAT   — the pattern reads gSat.
//                    The Saturation slider will visibly change this pattern.
//
//   PAT_USES_SPEED — the pattern calls showFrame(), so the Speed slider works.
//                    Patterns that don't call showFrame() run at a fixed speed.
//
//   PAT_USES_PAL   — the pattern calls fill_palette() or ColorFromPalette().
//                    The Palette selector will change this pattern's colors.
//
//   0              — the pattern ignores all four controls. It has hardcoded
//                    colors and no showFrame() call. This is intentional for
//                    some patterns (e.g. smoothWaves, mysticFlow) that use
//                    millis() for timing instead of a frame delay.
//
// =============================================================================

typedef void (*PatternHandler)();

struct PatternDefinition {
  const char    *name;     // Display name shown in the web UI
  PatternHandler handler;  // Pointer to the function in patterns.h
  uint8_t        flags;    // Bitmask of PAT_USES_* constants (see above)
};

// =============================================================================
// Pattern registry
// =============================================================================
// Add new patterns at the END. Never reorder or remove existing entries.

static const PatternDefinition kPatterns[] = {

  // ── Rainbow & Color Cycling (indices 0–8) ──────────────────────────────────
  { "Rainbow Cycle",         rainbowCycle,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Glitter",       rainbowGlitter,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Pulse",         rainbowPulse,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Sparkle",       rainbowSparkle,      PAT_USES_HUE                                  },
  { "Color Sweep",           colorSweep,          PAT_USES_SPEED                                },
  { "Color Fade",            colorFade,           PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Color Waves",           colorWaves,          PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Gradient Wave",         gradientWave,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Palette Cycle",         paletteCycle,        PAT_USES_PAL | PAT_USES_SPEED                 },

  // ── Movement & Chase (indices 9–16) ───────────────────────────────────────
  { "Moving Dot",            movingDot,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Comet Tail",            cometTail,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Sinelon",               sinelon,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Chase Rainbow",         chaseRainbow,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Cylon Bounce",          cylonBounce,         PAT_USES_SPEED                                },
  { "Bounce Comets",         bounceComets,        0 /* uses millis() for timing, no showFrame */},
  { "Wave Glide",            waveGlide,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Dot Trail",             dotTrail,            PAT_USES_HUE                                  },

  // ── Dynamic & Interactive (indices 17–25) ─────────────────────────────────
  { "Confetti",              confetti,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Confetti Pulse",        confettiPulse,       PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Juggle",                juggle,              PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Twinkle",               twinkle,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Twinkle Fade",          twinkle2,            PAT_USES_SPEED                                },
  { "Dazzle",                dazzle,              PAT_USES_SPEED                                },
  { "Glitter Fade",          glitterFade,         0 /* no showFrame — runs at loop() speed */   },
  { "Sparkle",               sparkle,             PAT_USES_SPEED                                },
  { "Pixel Pop",             pixelPop,            0 /* no showFrame — runs at loop() speed */   },

  // ── Breathing & Pulse (indices 26–31) ─────────────────────────────────────
  { "Breathing Effect",      breathingEffect,     PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Radiant Waves",         radiantWaves,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Center Pulse",          centerPulse,         PAT_USES_HUE                                  },
  { "Beat Wave",             beatWave,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Beat",          rainbowBeat,         PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Aura Glow",             auraGlow,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── Wave & Ripple (indices 32–35) ─────────────────────────────────────────
  { "Ripple",                ripple,              PAT_USES_SPEED                                },
  { "Ripple Stars",          rippleStars,         0 /* no showFrame — runs at loop() speed */   },
  { "Smooth Waves",          smoothWaves,         0 /* uses millis() for timing, no showFrame */},
  { "Waveform Shimmer",      waveformShimmer,     0 /* uses millis() for timing, no showFrame */},

  // ── Fire & Heat (indices 36–39) ───────────────────────────────────────────
  { "Fire Effect",           fireEffect,          PAT_USES_SPEED                                },
  { "Candle Flicker",        candleFlicker,       PAT_USES_SPEED                                },
  { "Lava Flow",             lavaFlow,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Noise Lava",            noiseLava,           0 /* uses millis() for timing, no showFrame */},

  // ── Electric & Neon (indices 40–42) ───────────────────────────────────────
  { "Electric Pulse",        electricPulse,       PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Neon Streaks",          neonStreaks,         0 /* no showFrame — runs at loop() speed */   },
  { "Hyperspace Tunnel",     hyperspaceTunnel,    PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── Nature & Weather (indices 43–47) ──────────────────────────────────────
  { "Aurora Borealis",       auroraBorealis,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Aurora Waves",          auroraWaves,         0 /* uses millis() for timing, no showFrame */},
  { "Ocean Current",         oceanCurrent,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Storm Pulse",           stormPulse,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Sunrise",               sunrise,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── Cosmic & Space (indices 48–53) ────────────────────────────────────────
  { "Galaxy Swirl",          galaxySwirl,         PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Plasma",                plasma,              PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Noise Rainbow",         noiseRainbow,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Perlin Noise",          perlinNoiseColors,   PAT_USES_SPEED                                },
  { "Noise Gradient",        noiseGradient,       PAT_USES_SPEED                                },
  { "Mystic Flow",           mysticFlow,          0 /* uses millis() for timing, no showFrame */},

  // ── Special & Festive (indices 54–66) ─────────────────────────────────────
  { "Candy Cane Twist",      candyCaneTwist,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Vortex Spin",           vortexSpin,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Matrix Rain",           matrixRain,          PAT_USES_SAT | PAT_USES_SPEED                 },
  { "Shimmer",               shimmer,             0 /* no showFrame — runs at loop() speed */   },
  { "Shimmer Curtain",       shimmerCurtain,      0 /* no showFrame — runs at loop() speed */   },
  { "Pastel Twinkle",        pastelTwinkle,       0 /* no showFrame — runs at loop() speed */   },
  { "Falling Stars",         fallingStars,        0 /* no showFrame — runs at loop() speed */   },
  { "Sine Spiral",           sineSpiral,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Color Tunnel",          colorTunnel,         PAT_USES_SPEED                                },
  { "Meteor Shower",         meteorShower,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Quantum Vortex",        quantumVortex,       PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Alien Aurora",          alienAurora,         PAT_USES_SPEED                                },
  { "Hypernova Burst",       hypernovaBurst,      PAT_USES_HUE | PAT_USES_SPEED                 },

  // ── Global Holidays (indices 67–76) ───────────────────────────────────────
  { "Christmas Snowflakes",  christmasSnowflakes, PAT_USES_SPEED },
  { "Christmas Lights",      christmasLights,     PAT_USES_SPEED },
  { "Halloween Ghosts",      halloweenGhosts,     PAT_USES_SPEED },
  { "Halloween Pumpkin",     halloweenPumpkin,    PAT_USES_SPEED },
  { "Easter Pastel Eggs",    easterPastelEggs,    PAT_USES_SPEED },
  { "New Year Fireworks",    newYearFireworks,    PAT_USES_SPEED },
  { "Diwali Diyas",          diwaliDiyas,         PAT_USES_SPEED },
  { "St Patricks Shamrock",  stPatricksShamrock,  PAT_USES_SPEED },
  { "Valentines Heartbeat",  valentinesHeartbeat, PAT_USES_SPEED },
  { "Fourth Of July",        fourthOfJuly,        PAT_USES_SPEED },

  // ── Indian Festivals & Seasons (indices 77–88) ────────────────────────────
  { "Holi Color Splash",     holiColorSplash,     PAT_USES_SPEED },
  { "Navratri Garba",        navratriGarba,       PAT_USES_SPEED },
  { "Rakhi Celebration",     rakhiCelebration,    PAT_USES_SPEED },
  { "Ganesh Aarti",          ganeshAarti,         PAT_USES_SPEED },
  { "Pongal Harvest",        pongalHarvest,       PAT_USES_SPEED },
  { "Monsoon Rains",         monsoonRains,        PAT_USES_SPEED },
  { "Onam Pookalam",         onamPookalam,        PAT_USES_SPEED },
  { "Janmashtami Peacock",   janmashtamiPeacock,  PAT_USES_SPEED },
  { "Baisakhi Fields",       baisakhiFields,      PAT_USES_SPEED },
  { "Makar Sankranti Kites", makarSankrantiKites, PAT_USES_SPEED },
  { "Durga Puja Dhak",       durgaPujaDhak,       PAT_USES_SPEED },
  { "Summer Mango Glow",     summerMangoGlow,     PAT_USES_SPEED },

  // ── Additional Holiday Variants (indices 89–91) ───────────────────────────
  { "Diwali Fireworks",      diwaliFireworks,     PAT_USES_SPEED },
  { "Easter Eggs",           easterEggs,          PAT_USES_SPEED },
  { "New Year Bubbles",      newYearBubbles,      PAT_USES_SPEED },

  // ── ADD NEW PATTERNS HERE ─────────────────────────────────────────────────
  // { "My Pattern",  myPattern,  PAT_USES_HUE | PAT_USES_SPEED },

};

// =============================================================================
// TOTAL_PATTERNS — computed automatically, never edit manually
// =============================================================================
const int TOTAL_PATTERNS = (int)(sizeof(kPatterns) / sizeof(kPatterns[0]));

// =============================================================================
// Accessors
// =============================================================================
// Use these instead of indexing kPatterns[] directly. They do bounds checking
// and are the single point of access for the rest of the codebase.

// Returns the display name of pattern at index, or "" if index is out of range.
const char *patternName(int index) {
  if (index < 0 || index >= TOTAL_PATTERNS) return "";
  return kPatterns[index].name;
}

// Returns the capability flags of pattern at index, or 0 if out of range.
uint8_t patternFlags(int index) {
  if (index < 0 || index >= TOTAL_PATTERNS) return 0;
  return kPatterns[index].flags;
}

// =============================================================================
// runCurrentPattern() — called every loop() iteration from fastLED.ino
// =============================================================================
void runCurrentPattern() {
  if (currentPattern >= 0 && currentPattern < TOTAL_PATTERNS) {
    kPatterns[currentPattern].handler();
  } else {
    // currentPattern is somehow out of range — fall back to the first pattern
    // rather than crashing or doing nothing.
    kPatterns[0].handler();
  }
}

// ── Pattern registry ──────────────────────────────────────────────────────────
// Each entry maps a stable index to a name, handler, and capability flags.
//
// Rules:
//   - Never reorder or remove entries; existing clients depend on stable indices.
//   - Append new patterns at the end only.
//   - Set flags honestly by inspecting the pattern implementation.

typedef void (*PatternHandler)();

struct PatternDefinition {
  const char    *name;
  PatternHandler handler;
  uint8_t        flags;   // bitmask of PAT_USES_* constants from constants.h
};

static const PatternDefinition kPatterns[] = {
  // ── 0–8: Rainbow & Color Cycling ─────────────────────────────────────────
  { "Rainbow Cycle",         rainbowCycle,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Glitter",       rainbowGlitter,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Pulse",         rainbowPulse,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Sparkle",       rainbowSparkle,      PAT_USES_HUE                                  },
  { "Color Sweep",           colorSweep,          PAT_USES_SPEED                                },
  { "Color Fade",            colorFade,           PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Color Waves",           colorWaves,          PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Gradient Wave",         gradientWave,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Palette Cycle",         paletteCycle,        PAT_USES_PAL | PAT_USES_SPEED                 },

  // ── 9–16: Movement & Chase ────────────────────────────────────────────────
  { "Moving Dot",            movingDot,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Comet Tail",            cometTail,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Sinelon",               sinelon,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Chase Rainbow",         chaseRainbow,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Cylon Bounce",          cylonBounce,         PAT_USES_SPEED                                },
  { "Bounce Comets",         bounceComets,        0                                             },
  { "Wave Glide",            waveGlide,           PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Dot Trail",             dotTrail,            PAT_USES_HUE                                  },

  // ── 17–25: Dynamic & Interactive ─────────────────────────────────────────
  { "Confetti",              confetti,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Confetti Pulse",        confettiPulse,       PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Juggle",                juggle,              PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Twinkle",               twinkle,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Twinkle Fade",          twinkle2,            PAT_USES_SPEED                                },
  { "Dazzle",                dazzle,              PAT_USES_SPEED                                },
  { "Glitter Fade",          glitterFade,         0                                             },
  { "Sparkle",               sparkle,             PAT_USES_SPEED                                },
  { "Pixel Pop",             pixelPop,            0                                             },

  // ── 26–31: Breathing & Pulse ──────────────────────────────────────────────
  { "Breathing Effect",      breathingEffect,     PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Radiant Waves",         radiantWaves,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Center Pulse",          centerPulse,         PAT_USES_HUE                                  },
  { "Beat Wave",             beatWave,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Rainbow Beat",          rainbowBeat,         PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Aura Glow",             auraGlow,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── 32–35: Wave & Ripple ──────────────────────────────────────────────────
  { "Ripple",                ripple,              PAT_USES_SPEED                                },
  { "Ripple Stars",          rippleStars,         0                                             },
  { "Smooth Waves",          smoothWaves,         0                                             },
  { "Waveform Shimmer",      waveformShimmer,     0                                             },

  // ── 36–39: Fire & Heat ────────────────────────────────────────────────────
  { "Fire Effect",           fireEffect,          PAT_USES_SPEED                                },
  { "Candle Flicker",        candleFlicker,       PAT_USES_SPEED                                },
  { "Lava Flow",             lavaFlow,            PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Noise Lava",            noiseLava,           0                                             },

  // ── 40–42: Electric & Neon ────────────────────────────────────────────────
  { "Electric Pulse",        electricPulse,       PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Neon Streaks",          neonStreaks,         0                                             },
  { "Hyperspace Tunnel",     hyperspaceTunnel,    PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── 43–47: Nature & Weather ───────────────────────────────────────────────
  { "Aurora Borealis",       auroraBorealis,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Aurora Waves",          auroraWaves,         0                                             },
  { "Ocean Current",         oceanCurrent,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Storm Pulse",           stormPulse,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Sunrise",               sunrise,             PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },

  // ── 48–53: Cosmic & Space ─────────────────────────────────────────────────
  { "Galaxy Swirl",          galaxySwirl,         PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Plasma",                plasma,              PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Noise Rainbow",         noiseRainbow,        PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Perlin Noise",          perlinNoiseColors,   PAT_USES_SPEED                                },
  { "Noise Gradient",        noiseGradient,       PAT_USES_SPEED                                },
  { "Mystic Flow",           mysticFlow,          0                                             },

  // ── 54–66: Special & Festive ──────────────────────────────────────────────
  { "Candy Cane Twist",      candyCaneTwist,      PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Vortex Spin",           vortexSpin,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Matrix Rain",           matrixRain,          PAT_USES_SAT | PAT_USES_SPEED                 },
  { "Shimmer",               shimmer,             0                                             },
  { "Shimmer Curtain",       shimmerCurtain,      0                                             },
  { "Pastel Twinkle",        pastelTwinkle,       0                                             },
  { "Falling Stars",         fallingStars,        0                                             },
  { "Sine Spiral",           sineSpiral,          PAT_USES_HUE | PAT_USES_SAT | PAT_USES_SPEED },
  { "Color Tunnel",          colorTunnel,         PAT_USES_SPEED                                },
  { "Meteor Shower",         meteorShower,        PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Quantum Vortex",        quantumVortex,       PAT_USES_HUE | PAT_USES_SPEED                 },
  { "Alien Aurora",          alienAurora,         PAT_USES_SPEED                                },
  { "Hypernova Burst",       hypernovaBurst,      PAT_USES_HUE | PAT_USES_SPEED                 },

  // ── 67–76: Global Holidays ────────────────────────────────────────────────
  { "Christmas Snowflakes",  christmasSnowflakes, PAT_USES_SPEED                                },
  { "Christmas Lights",      christmasLights,     PAT_USES_SPEED                                },
  { "Halloween Ghosts",      halloweenGhosts,     PAT_USES_SPEED                                },
  { "Halloween Pumpkin",     halloweenPumpkin,    PAT_USES_SPEED                                },
  { "Easter Pastel Eggs",    easterPastelEggs,    PAT_USES_SPEED                                },
  { "New Year Fireworks",    newYearFireworks,    PAT_USES_SPEED                                },
  { "Diwali Diyas",          diwaliDiyas,         PAT_USES_SPEED                                },
  { "St Patricks Shamrock",  stPatricksShamrock,  PAT_USES_SPEED                                },
  { "Valentines Heartbeat",  valentinesHeartbeat, PAT_USES_SPEED                                },
  { "Fourth Of July",        fourthOfJuly,        PAT_USES_SPEED                                },

  // ── 77–88: Indian Festivals & Seasons ────────────────────────────────────
  { "Holi Color Splash",     holiColorSplash,     PAT_USES_SPEED                                },
  { "Navratri Garba",        navratriGarba,       PAT_USES_SPEED                                },
  { "Rakhi Celebration",     rakhiCelebration,    PAT_USES_SPEED                                },
  { "Ganesh Aarti",          ganeshAarti,         PAT_USES_SPEED                                },
  { "Pongal Harvest",        pongalHarvest,       PAT_USES_SPEED                                },
  { "Monsoon Rains",         monsoonRains,        PAT_USES_SPEED                                },
  { "Onam Pookalam",         onamPookalam,        PAT_USES_SPEED                                },
  { "Janmashtami Peacock",   janmashtamiPeacock,  PAT_USES_SPEED                                },
  { "Baisakhi Fields",       baisakhiFields,      PAT_USES_SPEED                                },
  { "Makar Sankranti Kites", makarSankrantiKites, PAT_USES_SPEED                                },
  { "Durga Puja Dhak",       durgaPujaDhak,       PAT_USES_SPEED                                },
  { "Summer Mango Glow",     summerMangoGlow,     PAT_USES_SPEED                                },

  // ── 89–91: Additional holiday variants ───────────────────────────────────
  { "Diwali Fireworks",      diwaliFireworks,     PAT_USES_SPEED                                },
  { "Easter Eggs",           easterEggs,          PAT_USES_SPEED                                },
  { "New Year Bubbles",      newYearBubbles,      PAT_USES_SPEED                                },
};

// ── Single definitions of the extern symbols declared in constants.h ─────────
const int TOTAL_PATTERNS = (int)(sizeof(kPatterns) / sizeof(kPatterns[0]));

// ── Accessors ─────────────────────────────────────────────────────────────────
// All consumers read through these instead of a duplicate array.

const char *patternName(int index) {
  if (index < 0 || index >= TOTAL_PATTERNS) return "";
  return kPatterns[index].name;
}

uint8_t patternFlags(int index) {
  if (index < 0 || index >= TOTAL_PATTERNS) return 0;
  return kPatterns[index].flags;
}

// ── Dispatcher ────────────────────────────────────────────────────────────────

void runCurrentPattern() {
  if (currentPattern >= 0 && currentPattern < TOTAL_PATTERNS) {
    kPatterns[currentPattern].handler();
    return;
  }
  kPatterns[0].handler(); // safe fallback
}

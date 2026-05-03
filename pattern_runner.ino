void runCurrentPattern() {
  switch (currentPattern) {
    // Rainbow & Color Cycling (0-8)
    case 0: rainbowCycle(); break;
    case 1: rainbowGlitter(); break;
    case 2: rainbowPulse(); break;
    case 3: rainbowSparkle(); break;
    case 4: colorSweep(); break;
    case 5: colorFade(); break;
    case 6: colorWaves(); break;
    case 7: gradientWave(); break;
    case 8: paletteCycle(); break;
    
    // Movement & Chase (9-16)
    case 9: movingDot(); break;
    case 10: cometTail(); break;
    case 11: sinelon(); break;
    case 12: chaseRainbow(); break;
    case 13: cylonBounce(); break;
    case 14: bounceComets(); break;
    case 15: waveGlide(); break;
    case 16: dotTrail(); break;
    
    // Dynamic & Interactive (17-25)
    case 17: confetti(); break;
    case 18: confettiPulse(); break;
    case 19: juggle(); break;
    case 20: twinkle(); break;
    case 21: twinkle2(); break;
    case 22: dazzle(); break;
    case 23: glitterFade(); break;
    case 24: sparkle(); break;
    case 25: pixelPop(); break;
    
    // Breathing & Pulse (26-31)
    case 26: breathingEffect(); break;
    case 27: radiantWaves(); break;
    case 28: centerPulse(); break;
    case 29: beatWave(); break;
    case 30: rainbowBeat(); break;
    case 31: auraGlow(); break;
    
    // Wave & Ripple (32-35)
    case 32: ripple(); break;
    case 33: rippleStars(); break;
    case 34: smoothWaves(); break;
    case 35: waveformShimmer(); break;
    
    // Fire & Heat (36-39)
    case 36: fireEffect(); break;
    case 37: candleFlicker(); break;
    case 38: lavaFlow(); break;
    case 39: noiseLava(); break;
    
    // Electric & Neon (40-42)
    case 40: electricPulse(); break;
    case 41: neonStreaks(); break;
    case 42: hyperspaceTunnel(); break;
    
    // Nature & Weather (43-47)
    case 43: auroraBorealis(); break;
    case 44: auroraWaves(); break;
    case 45: oceanCurrent(); break;
    case 46: stormPulse(); break;
    case 47: sunrise(); break;
    
    // Cosmic & Space (48-53)
    case 48: galaxySwirl(); break;
    case 49: plasma(); break;
    case 50: noiseRainbow(); break;
    case 51: perlinNoiseColors(); break;
    case 52: noiseGradient(); break;
    case 53: mysticFlow(); break;
    
    // Special & Festive (54-60)
    case 54: candyCaneTwist(); break;
    case 55: vortexSpin(); break;
    case 56: matrixRain(); break;
    case 57: shimmer(); break;
    case 58: shimmerCurtain(); break;
    case 59: pastelTwinkle(); break;
    case 60: fallingStars(); break;
    
    default: rainbowCycle(); break;
  }
}

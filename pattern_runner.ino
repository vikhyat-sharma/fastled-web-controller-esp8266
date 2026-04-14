void runCurrentPattern() {
  switch (currentPattern) {
    case 0: rainbowCycle(); break;
    case 1: movingDot(); break;
    case 2: breathingEffect(); break;
    case 3: confetti(); break;
    case 4: sinelon(); break;
    case 5: juggle(); break;
    case 6: twinkle(); break;
    case 7: rainbowGlitter(); break;
    case 8: beatWave(); break;
    case 9: fireEffect(); break;
    case 10: ripple(); break;
    case 11: gradientWave(); break;
    case 12: rainbowPulse(); break;
    case 13: plasma(); break;
    case 14: noiseRainbow(); break;
    case 15: sunrise(); break;
    case 16: auroraBorealis(); break;
    case 17: oceanCurrent(); break;
    case 18: galaxySwirl(); break;
    case 19: stormPulse(); break;
    case 20: hyperspaceTunnel(); break;
    case 21: auraGlow(); break;
    case 22: waveGlide(); break;
    case 23: cometTail(); break;
    case 24: matrixRain(); break;
    case 25: lavaFlow(); break;
    case 26: vortexSpin(); break;
    case 27: electricPulse(); break;
    case 28: candyCaneTwist(); break;
    case 29: paletteCycle(); break;
    default: rainbowCycle(); break;
  }
}

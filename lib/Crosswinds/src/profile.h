// Build profile selection.
//
// The Makefiles pass the profile define on the command line (for example
// `-DSH_X4` for the Xteink X4 target). When building from the Arduino IDE,
// uncomment exactly one of the lines below instead. Leaving all of them
// commented selects the classic 480x320 ESP32-3248S035C touch panel.
#pragma once

// #define SH_X4      // Xteink X4: ESP32-C3, 4.26" 800x480 e-paper, buttons
// #define SH_HIRES   // 800x480 presentation profile (desktop preview)
// #define SH_EINK    // M5Stack PaperColor experiment (desktop preview)

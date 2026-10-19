#pragma once

// Measures how long a button has been held, for the quit gesture. Each loop step
// adds at most maxStepMs. The game blocks the loop for a second or two during an
// e-paper refresh, and that time must not count: pressing BACK and waiting for
// the screen to answer would otherwise quit the game.
struct HoldTimer {
  unsigned long lastMs = 0;
  unsigned long heldMs = 0;
  bool started = false;

  // Returns true once the button has been held for holdMs of responsive time.
  bool update(const bool down, const unsigned long nowMs, const unsigned long holdMs, const unsigned long maxStepMs) {
    const unsigned long elapsed = started ? nowMs - lastMs : 0;
    lastMs = nowMs;
    started = true;
    if (!down) {
      heldMs = 0;
      return false;
    }
    heldMs += elapsed < maxStepMs ? elapsed : maxStepMs;
    return heldMs >= holdMs;
  }

  void reset() {
    started = false;
    heldMs = 0;
  }
};

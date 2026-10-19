#pragma once

#ifdef CROSSWINDS_ENABLED

#include "HoldTimer.h"
#include "activities/Activity.h"

// Runs the Crosswinds game (lib/Crosswinds) as a full-screen activity.
// The game draws into the renderer's framebuffer itself and pushes its own
// frames, so render() is unused. Holding BACK returns to Home; the company
// is saved on every exit, including when CrossPoint goes to sleep.
class CrosswindsActivity final : public Activity {
  bool started = false;
  bool leaving = false;
  HoldTimer backHold;

 public:
  explicit CrosswindsActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
      : Activity("Crosswinds", renderer, mappedInput) {}
  void onEnter() override;
  void onExit() override;
  void loop() override;
};

#endif  // CROSSWINDS_ENABLED

#pragma once

#ifdef CROSSWINDS_ENABLED

#include "HoldTimer.h"
#include "activities/Activity.h"

// Runs the Crosswinds game (lib/Crosswinds) as a full-screen activity.
// The game draws into the renderer's framebuffer itself and pushes its own
// frames; render() requests a repaint after a reader popup closes. The company
// is saved on every exit, including when CrossPoint goes to sleep.
class CrosswindsActivity final : public Activity {
  bool started = false;
  bool asking = false;
  bool leaving = false;
  HoldTimer backHold;
  GfxRenderer::Orientation previousOrientation = GfxRenderer::Portrait;

  void onQuitAnswer(const ActivityResult& result);

 public:
  explicit CrosswindsActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
      : Activity("Crosswinds", renderer, mappedInput) {}
  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;
};

#endif  // CROSSWINDS_ENABLED

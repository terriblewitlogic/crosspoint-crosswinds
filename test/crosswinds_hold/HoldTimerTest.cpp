#include <gtest/gtest.h>

#include "src/activities/crosswinds/HoldTimer.h"

namespace {

constexpr unsigned long HOLD_MS = 1200;
constexpr unsigned long STEP_CAP_MS = 100;

}  // namespace

TEST(HoldTimerTest, SteadyHoldQuitsAfterTheHoldTime) {
  HoldTimer timer;
  unsigned long now = 0;
  bool quit = false;
  for (; now <= 1500 && !quit; now += 10) quit = timer.update(true, now, HOLD_MS, STEP_CAP_MS);
  EXPECT_TRUE(quit);
  EXPECT_GE(now, 1200u);
  EXPECT_LE(now, 1300u);
}

TEST(HoldTimerTest, ShortPressNeverQuits) {
  HoldTimer timer;
  unsigned long now = 0;
  for (; now < 200; now += 10) EXPECT_FALSE(timer.update(true, now, HOLD_MS, STEP_CAP_MS));
  for (; now < 3000; now += 10) EXPECT_FALSE(timer.update(false, now, HOLD_MS, STEP_CAP_MS));
}

TEST(HoldTimerTest, PressThenWaitThroughSlowRefreshesDoesNotQuit) {
  // Pressed, then the loop is blocked in refreshes of 2 s and 1.7 s while the
  // button is still down. Wall-clock time held is over 3.7 s.
  HoldTimer timer;
  EXPECT_FALSE(timer.update(true, 0, HOLD_MS, STEP_CAP_MS));
  EXPECT_FALSE(timer.update(true, 2000, HOLD_MS, STEP_CAP_MS));
  EXPECT_FALSE(timer.update(true, 3700, HOLD_MS, STEP_CAP_MS));
  EXPECT_FALSE(timer.update(true, 3710, HOLD_MS, STEP_CAP_MS));
}

TEST(HoldTimerTest, ReleaseResetsTheCount) {
  HoldTimer timer;
  unsigned long now = 0;
  for (; now < 1000; now += 10) EXPECT_FALSE(timer.update(true, now, HOLD_MS, STEP_CAP_MS));
  EXPECT_FALSE(timer.update(false, now, HOLD_MS, STEP_CAP_MS));
  now += 10;
  for (unsigned long start = now; now < start + 1000; now += 10) {
    EXPECT_FALSE(timer.update(true, now, HOLD_MS, STEP_CAP_MS));
  }
}

TEST(HoldTimerTest, HoldContinuesAcrossOneRefreshAndStillQuits) {
  // A deliberate hold: the first press redraws the screen (blocking 1.7 s), then
  // the loop runs freely and the hold completes.
  HoldTimer timer;
  unsigned long now = 0;
  EXPECT_FALSE(timer.update(true, now, HOLD_MS, STEP_CAP_MS));
  now += 1700;
  bool quit = false;
  for (int i = 0; i < 200 && !quit; ++i) {
    quit = timer.update(true, now, HOLD_MS, STEP_CAP_MS);
    now += 10;
  }
  EXPECT_TRUE(quit);
}

TEST(HoldTimerTest, ResetForgetsAnEarlierHold) {
  HoldTimer timer;
  unsigned long now = 0;
  for (; now < 1000; now += 10) timer.update(true, now, HOLD_MS, STEP_CAP_MS);
  timer.reset();
  EXPECT_FALSE(timer.update(true, now + 5000, HOLD_MS, STEP_CAP_MS));
}

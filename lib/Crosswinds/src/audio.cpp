#include "game.h"

// The Xteink X4 has no LED or speaker, and its GPIO4 is the display's
// data/command line, so signal feedback must stay off on that board.
#if defined(SH_X4)
#undef AUDIO_ENABLED
#define AUDIO_ENABLED 0
#endif
#ifndef AUDIO_ENABLED
#define AUDIO_ENABLED 1
#endif

#if AUDIO_ENABLED
static constexpr uint8_t LIGHT_PIN = 4;  // classic panel red LED, active-low
static uint8_t flashesLeft = 0;
static uint32_t flashUntil = 0;
static uint32_t nextFlash = 0;
static uint16_t onMs = 0, gapMs = 0;

static void light(bool on) { digitalWrite(LIGHT_PIN, on ? LOW : HIGH); }

static void flash_n(uint8_t count, uint16_t duration, uint16_t gap) {
  if (!g_campaign.soundOn || !count) return;
  light(true);
  flashesLeft = count - 1;
  onMs = duration;
  gapMs = gap;
  flashUntil = millis() + duration;
  nextFlash = flashUntil + gap;
}

void audio_begin() {
  pinMode(LIGHT_PIN, OUTPUT);
  light(false);
}

void audio_notice() { flash_n(1, 70, 0); }
void audio_alert() { flash_n(2, 60, 100); }

void audio_tick() {
  uint32_t now = millis();
  if (flashUntil && (int32_t)(now - flashUntil) >= 0) {
    light(false);
    flashUntil = 0;
  }
  if (flashesLeft && (int32_t)(now - nextFlash) >= 0) {
    --flashesLeft;
    light(true);
    flashUntil = now + onMs;
    nextFlash = flashUntil + gapMs;
  }
}
#else
void audio_begin() {}
void audio_tick() {}
void audio_notice() {}
void audio_alert() {}
#endif


// Crosswinds as an activity inside the CrossPoint reader firmware.
//
// This replaces ships.ino and board_x4.cpp for the SH_CROSSPOINT profile:
// CrossPoint owns boot, power, sleep and the button sampler, and calls these
// three functions from its activity. The game state is loaded from the save
// on every start, so leaving the game and coming back resumes the company.
#if defined(SH_CROSSPOINT)
#include <LittleFS.h>
#include <esp_random.h>
#include "game.h"
#include "display.h"
#include "x4.h"

bool g_bootResumed = false;

static uint32_t lastInput = 0;
static uint32_t lastSimulation = 0;
static uint32_t lastAutosave = 0;
static bool activeClockReady = false;

static void advance_live_clock(uint32_t now) {
  if (!activeClockReady || now - lastSimulation < 1000) return;
  uint32_t elapsedSeconds = (now - lastSimulation) / 1000;
  if (elapsedSeconds > 60) elapsedSeconds = 60;
  uint32_t scale = g_campaign.timeScale ? g_campaign.timeScale : 1;
  uint8_t oldStates[MAX_SHIPS];
  uint8_t oldShipCount = g_campaign.shipCount;
  for (int i = 0; i < oldShipCount; ++i) oldStates[i] = g_ships[i].state;
  sim_advance_minutes(elapsedSeconds * SIM_WORLD_MINUTES_PER_REAL_SECOND * scale);
  bool stateChanged = oldShipCount != g_campaign.shipCount;
  for (int i = 0; i < oldShipCount && !stateChanged; ++i)
    stateChanged = oldStates[i] != g_ships[i].state;
  render_mark_dirty(0, 0, IW, stateChanged ? IH : ACTIONBAR_Y);
  lastSimulation += elapsedSeconds * 1000;
}

void board_x4_sync_active_clock() { advance_live_clock(millis()); }

bool crosswinds_start() {
  if (!display_begin()) { Serial.println("Crosswinds: no framebuffer"); return false; }
  // CrossPoint keeps its own files on the SD card and leaves the flash data
  // partition unmounted; the first start formats it for the game's saves.
  if (!LittleFS.begin(true) || !save_begin()) Serial.println("Crosswinds: persistence unavailable");

  if (save_exists() && save_read()) {
    g_bootResumed = true;
    Serial.println("Crosswinds: resumed company.");
  } else {
    bool unreadable = save_exists();
    if (unreadable) save_quarantine();
    uint32_t seed = esp_random();
    sim_new_campaign(seed ? seed : 0x51A17EEDu);
    if (unreadable) journal_add(2, "Save could not be read; started a new company.");
    save_write();
    Serial.printf("Crosswinds: new company, seed %08lx\n", (unsigned long)seed);
  }

  x4_prepare_return(g_bootResumed);
  lastInput = lastSimulation = lastAutosave = millis();
  activeClockReady = true;
  render_frame();
  x4_input_ready();
  return true;
}

void crosswinds_tick() {
  uint32_t now = millis();
  if (now - lastInput >= 33) {
    input_update();
    now = millis();
    lastInput = now;
  }
  advance_live_clock(now);
  if (now - lastAutosave >= 60000) {
    save_write();
    lastAutosave = now;
  }
  render_tick();
  if (g_ui.dirty) render_frame();
}

void crosswinds_stop() {
  if (!activeClockReady) return;
  advance_live_clock(millis());
  save_write();
  activeClockReady = false;
}

#endif

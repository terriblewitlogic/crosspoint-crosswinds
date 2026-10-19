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
#include "save_fs.h"
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

// The first CrossPoint builds kept saves in LittleFS on CrossPoint's unused
// flash data partition. A company found there moves to the SD card once; the
// flash copies are then renamed so a later campaign reset cannot revive them.
static const char* const FLASH_SAVES[] = {
  "/sh_company.sav", "/sh_company.bak",
  "/sh_fleet.sav", "/sh_ports.sav", "/sh_jobs.sav", "/sh_log.sav", "/sh_core.sav",
  "/sh_fleet.bak", "/sh_ports.bak", "/sh_jobs.bak", "/sh_log.bak", "/sh_core.bak"
};

static void import_flash_company() {
  if (save_exists()) return;              // the SD card already holds a company
  if (!LittleFS.begin(false)) return;     // never format: no filesystem, nothing to import
  bool copied = false, failed = false;
  for (const char* path : FLASH_SAVES) {
    if (!LittleFS.exists(path)) continue;
    File in = LittleFS.open(path, "r");
    SaveFile out = SAVE_FS.open(path, "w");
    bool ok = in && out;
    uint8_t chunk[256];
    size_t got;
    while (ok && (got = in.read(chunk, sizeof(chunk))) > 0) ok = out.write(chunk, got) == got;
    in.close();
    out.close();
    if (ok) copied = true;
    else { failed = true; SAVE_FS.remove(path); }
  }
  if (copied && !failed && save_exists()) {
    for (const char* path : FLASH_SAVES) {
      if (!LittleFS.exists(path)) continue;
      char done[40];
      snprintf(done, sizeof(done), "%s.imported", path);
      LittleFS.remove(done);
      LittleFS.rename(path, done);
    }
    Serial.println("Crosswinds: moved the company from flash to the SD card.");
  } else if (copied || failed) {
    // All or nothing: drop partial copies so the next start tries again.
    for (const char* path : FLASH_SAVES) SAVE_FS.remove(path);
    Serial.println("Crosswinds: could not move the flash company; it stays in flash.");
  }
  LittleFS.end();
}

bool crosswinds_start() {
  g_bootResumed = false;  // every launch decides afresh
  if (!display_begin()) { Serial.println("Crosswinds: no framebuffer"); return false; }
  if (save_begin()) import_flash_company();
  else Serial.println("Crosswinds: persistence unavailable");

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

  // Opening the game from CrossPoint's Home is a deliberate start, so the
  // title comes first even when a Company update is waiting behind it.
  x4_prepare_return(g_bootResumed, true, true);
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

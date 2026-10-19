#include "CrosswindsActivity.h"

#ifdef CROSSWINDS_ENABLED

#include <HalGPIO.h>
#include <HalPowerManager.h>
#include <HalStorage.h>
#include <Logging.h>

#include <cstdio>
#include <new>

// Game entry points (lib/Crosswinds/src/crosspoint_main.cpp). The game's headers
// stay out of this file because they rename `display`, which CrossPoint also uses.
bool crosswinds_start();
void crosswinds_tick();
void crosswinds_stop();

namespace {

constexpr unsigned long EXIT_HOLD_MS = 1200;
constexpr uint8_t GAME_BUTTONS = 6;  // BACK, CONFIRM, LEFT, RIGHT, UP, DOWN; POWER stays CrossPoint's

GfxRenderer* bridgeRenderer = nullptr;
bool exitRequested = false;

// Presses since the game last looked, oldest first.
uint8_t queue[8];
uint8_t queueCount = 0;

void pushPress(const uint8_t button) {
  if (queueCount == sizeof(queue)) return;
  queue[queueCount++] = button;
}

}  // namespace

// ---------------------------------------------------------------------------
// The game's display and board layer (declared in its lgfx_config.h and x4.h)
// ---------------------------------------------------------------------------

uint8_t* crosspoint_framebuffer() { return bridgeRenderer ? bridgeRenderer->getFrameBuffer() : nullptr; }

// Clean refreshes use the half refresh: about 1.7 s on the X4, against about
// 3.8 s for the full waveform.
void crosspoint_display_push(const bool full) {
  if (!bridgeRenderer) return;
  bridgeRenderer->displayBuffer(full ? HalDisplay::HALF_REFRESH : HalDisplay::FAST_REFRESH);
}

void board_x4_begin_early() {}
void board_x4_begin() {}
void board_x4_input_ready() {}
bool board_x4_background_wake() { return false; }
void board_x4_resume_standby(bool) {}
void board_x4_tick() {}
bool board_x4_sleep_soon() { return false; }
bool board_x4_asleep() { return false; }
bool board_x4_buffered_input() { return true; }

uint8_t board_x4_read_buttons() {
  uint8_t mask = 0;
  for (uint8_t b = 0; b < GAME_BUTTONS; ++b) {
    if (gpio.isPressed(b)) mask |= static_cast<uint8_t>(1u << b);
  }
  return mask;
}

int board_x4_take_button_event() {
  if (queueCount == 0) return -1;
  const int button = queue[0];
  for (uint8_t i = 1; i < queueCount; ++i) queue[i - 1] = queue[i];
  --queueCount;
  return button;
}

bool board_x4_button_pending(const uint8_t button) {
  for (uint8_t i = 0; i < queueCount; ++i) {
    if (queue[i] == button) return true;
  }
  return false;
}

void board_x4_discard_button_events() { queueCount = 0; }

int board_x4_battery_percent() { return powerManager.getBatteryPercentage(); }
bool board_x4_usb_connected() { return gpio.isUsbConnected(); }

// The game's POWER action and its low-battery shutdown both hand control
// back to CrossPoint, which owns sleep and power.
void board_x4_power_off() { exitRequested = true; }
void board_x4_shutdown() { exitRequested = true; }

// ---------------------------------------------------------------------------
// Save storage for the game's save_fs.h: files in one hidden SD folder, through
// HalStorage so card access is serialized with the reader.
// ---------------------------------------------------------------------------

struct CrosspointFile {
  HalFile file;
};

namespace {

constexpr char SAVE_DIR[] = "/.crosspoint/crosswinds";

bool savePath(const char* path, char* out, const size_t cap) {
  const int n = snprintf(out, cap, "%s%s", SAVE_DIR, path);
  return n > 0 && static_cast<size_t>(n) < cap;
}

}  // namespace

bool crosspoint_fs_begin() {
  if (!Storage.ready()) return false;
  if (!Storage.exists(SAVE_DIR)) Storage.mkdir(SAVE_DIR);
  return Storage.exists(SAVE_DIR);
}

bool crosspoint_fs_exists(const char* path) {
  char full[96];
  return savePath(path, full, sizeof(full)) && Storage.exists(full);
}

bool crosspoint_fs_remove(const char* path) {
  char full[96];
  return savePath(path, full, sizeof(full)) && Storage.remove(full);
}

bool crosspoint_fs_rename(const char* from, const char* to) {
  char fullFrom[96], fullTo[96];
  return savePath(from, fullFrom, sizeof(fullFrom)) && savePath(to, fullTo, sizeof(fullTo)) &&
         Storage.rename(fullFrom, fullTo);
}

CrosspointFile* crosspoint_fs_open(const char* path, const char* mode) {
  char full[96];
  if (!savePath(path, full, sizeof(full))) return nullptr;
  const oflag_t flags = mode[0] == 'w' ? (O_WRONLY | O_CREAT | O_TRUNC) : (mode[1] == '+' ? O_RDWR : O_RDONLY);
  HalFile file = Storage.open(full, flags);
  if (!file) return nullptr;
  // One handle per open save file, freed by crosspoint_fs_close.
  auto* handle = new (std::nothrow) CrosspointFile{std::move(file)};
  if (!handle) LOG_ERR("CW", "OOM opening %s", full);
  return handle;
}

size_t crosspoint_fs_read(CrosspointFile* file, void* buf, const size_t len) {
  const int got = file->file.read(buf, len);
  return got > 0 ? static_cast<size_t>(got) : 0;
}

size_t crosspoint_fs_write(CrosspointFile* file, const void* buf, const size_t len) {
  return file->file.write(buf, len);
}

bool crosspoint_fs_seek(CrosspointFile* file, const uint32_t pos) { return file->file.seek(pos); }

void crosspoint_fs_flush(CrosspointFile* file) { file->file.flush(); }

void crosspoint_fs_close(CrosspointFile* file) {
  file->file.close();
  delete file;
}

// ---------------------------------------------------------------------------
// Activity
// ---------------------------------------------------------------------------

void CrosswindsActivity::onEnter() {
  Activity::onEnter();
  bridgeRenderer = &renderer;
  exitRequested = false;
  leaving = false;
  queueCount = 0;
  backDownSince = 0;
  {
    RenderLock lock(*this);
    started = crosswinds_start();
  }
  if (!started) {
    LOG_ERR("CW", "Crosswinds could not start");
    activityManager.goToFullScreenMessage("Not enough memory to open Crosswinds. Close other features and try again.");
  }
}

void CrosswindsActivity::onExit() {
  // ActivityManager holds the render lock here.
  if (started) crosswinds_stop();
  started = false;
  bridgeRenderer = nullptr;
  queueCount = 0;
  Activity::onExit();
}

void CrosswindsActivity::loop() {
  if (!started) return;

  if (leaving) {
    // Home acts on button releases. Wait until every button is up so the press
    // that quit cannot select the Crosswinds row again or open the last book.
    for (uint8_t b = 0; b < GAME_BUTTONS; ++b) {
      if (gpio.isPressed(b) || gpio.wasReleased(b)) return;
    }
    onGoHome();
    return;
  }

  for (uint8_t b = 0; b < GAME_BUTTONS; ++b) {
    if (gpio.wasPressed(b)) pushPress(b);
  }

  if (gpio.isPressed(HalGPIO::BTN_BACK)) {
    if (backDownSince == 0) {
      backDownSince = millis();
    } else if (millis() - backDownSince >= EXIT_HOLD_MS) {
      exitRequested = true;
    }
  } else {
    backDownSince = 0;
  }

  if (exitRequested) {
    exitRequested = false;
    leaving = true;
    return;
  }

  RenderLock lock(*this);
  crosswinds_tick();
}

#endif  // CROSSWINDS_ENABLED

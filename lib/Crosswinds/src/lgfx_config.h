// LovyanGFX configuration shared by the classic ESP32 panel, the Xteink X4
// e-paper board, and the SDL desktop host.
#pragma once
#include "profile.h"

#ifndef LGFX_USE_V1
#define LGFX_USE_V1
#endif
#include <LovyanGFX.hpp>

#if defined(LGFX_SDL)
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_sdl panel;
 public:
  LGFX() {
    auto c = panel.config();
#if defined(SH_EINK)
    c.memory_width = c.panel_width = 400;
    c.memory_height = c.panel_height = 600;
#elif defined(SH_X4)
    c.memory_width = c.panel_width = 800;
    c.memory_height = c.panel_height = 480;
#elif defined(SH_HIRES)
    c.memory_width = c.panel_width = 800;
    c.memory_height = c.panel_height = 480;
#else
    c.memory_width = c.panel_width = 480;
    c.memory_height = c.panel_height = 320;
#endif
    panel.config(c);
#if defined(SH_EINK)
    panel.setScaling(1, 1);
    panel.setWindowTitle("Ships PaperColor Preview");
#elif defined(SH_X4)
    panel.setScaling(1, 1);
    panel.setWindowTitle("Ships Xteink X4 Preview");
#elif defined(SH_HIRES)
    panel.setScaling(1, 1);
    panel.setWindowTitle("Ships HD Preview");
#else
    panel.setScaling(2, 2);
    panel.setWindowTitle("Ships");
#endif
    setPanel(&panel);
  }
};

static constexpr int DISPLAY_ROTATION = 0;
extern LGFX display;
static inline void board_backlight_begin() { display.setBrightness(255); }
static inline void board_backlight_set(uint8_t pct) {
  display.setBrightness((uint32_t)pct * 255 / 100);
}

#elif defined(SH_CROSSPOINT)
// Inside the CrossPoint reader firmware. The game draws straight into
// CrossPoint's 800x480 1-bit framebuffer (panel-native, MSB first, white = 1)
// through a LovyanGFX sprite, and the CrossPoint side pushes it to the panel.
// CrossPoint already owns a global named `display`, so the game's is renamed;
// LovyanGFX's own display() member is declared above and keeps its name.
uint8_t* crosspoint_framebuffer();
void crosspoint_display_push(bool full);

class LGFX : public lgfx::LGFX_Sprite {
  lgfx::epd_mode_t mode = lgfx::epd_mode_t::epd_quality;
 public:
  bool init() {
    uint8_t* fb = crosspoint_framebuffer();
    if (!fb) return false;
    setBuffer(fb, 800, 480, 1);  // grayscale_1bit: white -> 1, as CrossPoint stores it
    return true;
  }
  void setAutoDisplay(bool) {}
  void setEpdMode(lgfx::epd_mode_t m) { mode = m; }
  lgfx::epd_mode_t getEpdMode() const { return mode; }
  void cw_display(int32_t, int32_t, int32_t, int32_t) {
    crosspoint_display_push(mode != lgfx::epd_mode_t::epd_fast);
  }
  void cw_display() { cw_display(0, 0, 0, 0); }
  void waitDisplay() {}
  bool getTouch(int32_t*, int32_t*) { return false; }  // buttons only
};

#define display cw_display
static constexpr int DISPLAY_ROTATION = 0;
extern LGFX display;
static inline void board_backlight_begin() {}
static inline void board_backlight_set(uint8_t) {}

#elif defined(SH_X4)
#include "x4_pins.h"
#include <lgfx/v1/panel/Panel_SSD1677.hpp>  // not part of the umbrella header

// Xteink X4: GDEQ0426T82 4.26" 800x480 e-paper on an SSD1677 controller, SPI2
// of the ESP32-C3. The panel is landscape-native: rotation 0 puts the four
// front buttons along the right edge of the picture (reader turned
// anticlockwise from upright). Rotation 2 flips the picture for holding the
// reader the other way, with the buttons on the left; rotation 3 is the
// upright portrait orientation CrossPoint uses.
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_SSD1677 panel;
  lgfx::Bus_SPI bus;
 public:
  LGFX() {
    {
      auto c = bus.config();
      c.spi_host = SPI2_HOST;
      c.spi_mode = 0;
      c.freq_write = 20000000;  // SSD1677 datasheet maximum for writes
      c.freq_read = 8000000;
      c.spi_3wire = true;       // write-only panel, MOSI only
      c.use_lock = true;
      c.dma_channel = SPI_DMA_CH_AUTO;
      c.pin_sclk = X4_PIN_EPD_SCLK;
      c.pin_mosi = X4_PIN_EPD_MOSI;
      c.pin_miso = -1;
      c.pin_dc = X4_PIN_EPD_DC;
      bus.config(c);
      panel.setBus(&bus);
    }
    {
      auto c = panel.config();
      c.pin_cs = X4_PIN_EPD_CS;
      c.pin_rst = X4_PIN_EPD_RST;
      c.pin_busy = X4_PIN_EPD_BUSY;
      c.memory_width = 800;
      c.memory_height = 480;
      c.panel_width = 800;
      c.panel_height = 480;
      c.offset_x = 0;
      c.offset_y = 0;
      c.offset_rotation = 0;
      c.readable = false;
      c.invert = false;
      c.rgb_order = false;
      c.dlen_16bit = false;
      c.bus_shared = true;      // the SD card socket shares SCLK/MOSI
      panel.config(c);
    }
    setPanel(&panel);
  }
};

static constexpr int DISPLAY_ROTATION = 0;
extern LGFX display;
static inline void board_backlight_begin() {}
static inline void board_backlight_set(uint8_t) {}

#else

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 panel;
  lgfx::Bus_SPI bus;
  lgfx::Light_PWM light;
  lgfx::Touch_GT911 touch;
 public:
  LGFX() {
    {
      auto c = bus.config();
      c.spi_host = VSPI_HOST;
      c.spi_mode = 0;
      c.freq_write = 40000000;
      c.freq_read = 16000000;
      c.spi_3wire = true;
      c.use_lock = true;
      c.dma_channel = SPI_DMA_CH_AUTO;
      c.pin_sclk = 14;
      c.pin_mosi = 13;
      c.pin_miso = 12;
      c.pin_dc = 2;
      bus.config(c);
      panel.setBus(&bus);
    }
    {
      auto c = panel.config();
      c.pin_cs = 15;
      c.pin_rst = -1;
      c.pin_busy = -1;
      c.panel_width = 320;
      c.panel_height = 480;
      c.offset_x = 0;
      c.offset_y = 0;
      c.offset_rotation = 0;
      c.dummy_read_pixel = 8;
      c.dummy_read_bits = 1;
      c.readable = true;
      c.invert = false;
      c.rgb_order = false;
      c.dlen_16bit = false;
      c.bus_shared = true;
      panel.config(c);
    }
    {
      auto c = light.config();
      c.pin_bl = 27;
      c.invert = false;
      c.freq = 44100;
      c.pwm_channel = 7;
      light.config(c);
      panel.setLight(&light);
    }
    {
      auto c = touch.config();
      c.x_min = 0; c.x_max = 319;
      c.y_min = 0; c.y_max = 479;
      c.pin_int = -1;
      c.pin_rst = 25;
      c.bus_shared = false;
      c.offset_rotation = 0;
      c.i2c_port = 1;
      c.i2c_addr = 0x5D;
      c.pin_sda = 33;
      c.pin_scl = 32;
      c.freq = 400000;
      touch.config(c);
      panel.setTouch(&touch);
    }
    setPanel(&panel);
  }
};

static constexpr int DISPLAY_ROTATION = 1;
extern LGFX display;
static inline void board_backlight_begin() { display.setBrightness(255); }
static inline void board_backlight_set(uint8_t pct) {
  display.setBrightness((uint32_t)pct * 255 / 100);
}
#endif

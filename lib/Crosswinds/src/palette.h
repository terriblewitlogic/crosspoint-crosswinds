#pragma once
#include <Arduino.h>

#define RGB(r, g, b) ((uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)))

#if defined(SH_EINK)
// E Ink Spectra 6 has exactly these six fixed colors; every render_eink.cpp
// draw call uses these names directly rather than the blended palette below,
// which assumes a much larger color space.
static constexpr uint16_t E_BLACK  = RGB(0, 0, 0);
static constexpr uint16_t E_WHITE  = RGB(255, 255, 255);
static constexpr uint16_t E_RED    = RGB(255, 0, 0);
static constexpr uint16_t E_YELLOW = RGB(255, 255, 0);
static constexpr uint16_t E_BLUE   = RGB(0, 0, 255);
static constexpr uint16_t E_GREEN  = RGB(0, 255, 0);
#endif

static constexpr uint16_t C_DEEP       = RGB(10, 88, 156);
static constexpr uint16_t C_OCEAN      = RGB(16, 137, 198);
static constexpr uint16_t C_SHOAL      = RGB(67, 183, 214);
static constexpr uint16_t C_WAKE       = RGB(210, 242, 239);
static constexpr uint16_t C_LAND       = RGB(103, 174, 91);
static constexpr uint16_t C_HIGHLAND   = RGB(151, 157, 82);
static constexpr uint16_t C_DESERT     = RGB(227, 193, 103);
static constexpr uint16_t C_PARCH      = RGB(255, 229, 154);
static constexpr uint16_t C_PARCH_D    = RGB(235, 181, 80);
static constexpr uint16_t C_PANEL      = RGB(255, 213, 116);
static constexpr uint16_t C_INK        = RGB(43, 45, 55);
static constexpr uint16_t C_INK2       = RGB(93, 91, 102);
static constexpr uint16_t C_BRASS      = RGB(236, 161, 47);
static constexpr uint16_t C_GOLD       = RGB(255, 220, 52);
static constexpr uint16_t C_GOLD_D     = RGB(176, 112, 12);
static constexpr uint16_t C_WHITE      = RGB(255, 249, 221);
static constexpr uint16_t C_BLACK      = RGB(19, 22, 30);
static constexpr uint16_t C_RED        = RGB(224, 68, 63);
static constexpr uint16_t C_GREEN      = RGB(67, 180, 100);
static constexpr uint16_t C_STORM      = RGB(90, 97, 122);
static constexpr uint16_t C_ROUTE      = RGB(255, 236, 74);
static constexpr uint16_t C_LOCKED     = RGB(130, 140, 151);
static constexpr uint16_t C_SAIL       = RGB(255, 247, 213);
static constexpr uint16_t C_HULL       = RGB(129, 80, 46);

static constexpr uint16_t C_SKY        = RGB(112, 207, 235);
static constexpr uint16_t C_CORAL      = RGB(247, 111, 78);
static constexpr uint16_t C_MINT       = RGB(122, 219, 136);
static constexpr uint16_t C_BLUE       = RGB(38, 118, 203);
static constexpr uint16_t C_BLUE_D     = RGB(20, 66, 142);
static constexpr uint16_t C_LILAC      = RGB(151, 118, 222);

static constexpr uint16_t SEM_GOOD     = C_GREEN;
static constexpr uint16_t SEM_DANGER   = C_RED;
static constexpr uint16_t SEM_TREASURE = C_GOLD;
static constexpr uint16_t SEM_ACCENT   = C_ROUTE;
static constexpr uint16_t SEM_FRAME    = C_BRASS;
static constexpr uint16_t SEM_MUTED    = C_INK2;

# X4 Pro preview

Crosswinds inside CrossPoint, built for the Xteink X4 Pro's ESP32-S3, 16 MB flash,
8 MB PSRAM and touch screen. The game runs at 800 × 480 in landscape.

## Install

This preview is an application firmware image for an X4 Pro already running
CrossPoint with its standard partition layout. It replaces the reader firmware
with this CrossPoint fork, which includes Crosswinds.

1. Back up the SD card, including `/.crosspoint/crosswinds` if present.
2. Copy `crosswinds-2026.11.0rc-x4pro.bin` onto the card.
3. Open **Settings → System → SD Card Firmware Update**, select the file and confirm.
4. After restarting, open **Crosswinds** from Home.

Do not use the X3/X4 ESP32-C3 binary on the Pro. This preview contains no bootloader
or partition table; installation over stock Xteink firmware is not verified.
Download it from the [2026.11.0rc preview release](https://github.com/terriblewitlogic/crosspoint-crosswinds/releases/tag/2026.11.0rc).
Install prereleases from the SD card; Check for Updates uses stable releases.

## Controls

- Tap captains, list rows and buttons.
- Tap **− / +** to set a trade amount, then **BUY / SELL** to confirm.
- Swipe up or down on lists to page; drag the chart to pan after release.
- Use the side keys for ports and pages.
- Tap **BACK** or swipe inward from the left edge to return.
- Choose **QUIT** in the Company Menu to save and return Home.
- CrossPoint's Home and frontlight controls remain available.

Saves use `/.crosspoint/crosswinds` on the SD card.

## Validation

The ESP32-S3 release target builds. The SDL touch test covers captain selection,
naming, quantity adjustment, purchase confirmation, the first voyage, selling,
save reload and skipping the tutorial. Its source is in `test/crosswinds_touch`.

No X4 Pro hardware was available for testing. Check touch alignment, display
refresh, side keys, Home/quit, sleep and reopening after the frontlight panel on
the device before publishing this as a tested release. Open the game from each
reader orientation (portrait, inverted portrait, landscape clockwise and
counterclockwise): it should use landscape and restore the reader orientation
on exit. Repeat entry/exit and check that free memory does not steadily decrease.

## Build

```sh
git submodule update --init --recursive
pio run -e x4pro-gh_release
```

Output: `.pio/build/x4pro-gh_release/firmware.bin`. Use the pioarduino core pinned
in CI. The SDK remains pinned to `5e94fe7480f019c89fbf691aef87523eb1b46704`.

# Crosswinds for Xteink X4 and X4 Pro

A maritime trading game inside [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader).
Buy cargo, sail between ports and sell it for a profit. Open **Crosswinds** from the Home screen.

## Downloads

| Device | Controls | Download | Status |
| --- | --- | --- | --- |
| X4 · ESP32-C3 | Buttons | [2026.10.0 stable](https://github.com/terriblewitlogic/crosspoint-crosswinds/releases/download/2026.10.0/crosswinds-2026.10.0-x3-x4.bin) | Tested on one X4 |
| X4 Pro · ESP32-S3 | Touch and side keys | [2026.11.0rc preview](https://github.com/terriblewitlogic/crosspoint-crosswinds/releases/download/2026.11.0rc/crosswinds-2026.11.0rc-x4pro.bin) | Build and simulator tested; not hardware-tested |

The binaries are device-specific. The `x3-x4` image hides the game on X3.
The [preview release](https://github.com/terriblewitlogic/crosspoint-crosswinds/releases/tag/2026.11.0rc)
also includes an X4 build for testing.

## Install

On a device already running CrossPoint, copy the matching `.bin` to the SD card,
then select **Settings → System → SD Card Firmware Update**. Back up your SD card first.
This replaces the reader firmware with this CrossPoint fork; it is not a separate app.

Device instructions: [X4](docs/X4_INSTALL.md) · [X4 Pro](docs/X4_PRO.md).
The Pro preview has not been verified over stock Xteink firmware.

## Controls and saves

- **X4:** hold the reader in landscape with the front buttons on the right. From top to bottom: UP, DOWN, OK, BACK. The side rocker changes ports and pages.
- **X4 Pro:** tap cards, rows and buttons. Use **− / +** for trade quantities. Swipe lists to page; drag a zoomed chart to pan. Side keys change ports and pages.
- **Quit:** choose **QUIT** in the Company Menu. On X4, holding BACK also opens the quit dialog.
- **Saves:** `/.crosspoint/crosswinds` on the SD card. Copy that folder to back up your company.

English only, no sound. Ships continue sailing while the game is closed in the preview,
until the reader sleeps or restarts. In 2026.10.0, time advances only while the game is open.

## Build

```sh
git clone --recurse-submodules https://github.com/terriblewitlogic/crosspoint-crosswinds.git
cd crosspoint-crosswinds
pio run -e gh_release             # X4
pio run -e x4pro-gh_release       # X4 Pro
```

Use pioarduino 6.1.19, as pinned in CI. Binaries are under `.pio/build/<environment>/firmware.bin`.
See [port and source-sync notes](docs/CROSSWINDS.md) and [touch tests](test/crosswinds_touch/README.md).

## Related projects

- [Crosswinds](https://github.com/terriblewitlogic/crosswinds): standalone X4 firmware.
- [Crosswinds ESP32](https://github.com/terriblewitlogic/crosswinds-esp32): ESP32 touchscreen version; source of the Pro touch controls.

MIT. See [LICENSE](LICENSE) and [NOTICE](NOTICE) for credits and AI assistance disclosures.
This is an unofficial CrossPoint fork. Report Crosswinds issues here.

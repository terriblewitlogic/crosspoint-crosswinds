# How Crosswinds runs inside CrossPoint

The game is an `Activity` (`src/activities/crosswinds/CrosswindsActivity.*`) that
opens from a Home menu entry. Everything else about the firmware is CrossPoint's.

## Layout

- `lib/Crosswinds/src` holds the game. It is a copy: run
  `scripts/sync_crosswinds.sh /path/to/game-checkout` to refresh it. The script
  leaves out the other devices' renderers and art, and rewrites the headers of the
  generated art files, whose generator scripts live in the game's repository.
- `crosspoint_main.cpp` replaces the standalone `ships.ino` and `board_x4.cpp`. It
  exposes `crosswinds_start()`, `crosswinds_tick()` and `crosswinds_stop()`.
- `CrosswindsActivity.cpp` supplies what the game expects from its board layer:
  the framebuffer, button events, battery and USB state, and file storage.
- The game is compiled only into the `default`, `gh_release` and `gh_release_rc`
  images (`-DCROSSWINDS_ENABLED=1`), and Home shows the entry only on an X4.

## Display

The game draws into CrossPoint's 800x480 1-bit framebuffer through a LovyanGFX
sprite (`lib/Crosswinds/src/lgfx_config.h`). Two LovyanGFX behaviors needed work-arounds:

- `setBuffer(..., bpp)` does not change the sprite's panel depth, so the depth is
  set first. Without that, the first fill writes past the 48,000-byte buffer.
- Sprites under 8 bits per pixel copy palette indexes instead of looking up colors,
  which swapped ink and paper in the art. The shim does its own lookup, and
  `make -C tools/host crosspoint-blit-test` in the game repository compares it with
  LovyanGFX's 16-bit path.

Clean refreshes use CrossPoint's half refresh (about 1.7 s on the X4).

## Memory

Build sizes from `pio run -e gh_release`, for CrossPoint 1.6.5 and for this repository:

| | CrossPoint 1.6.5 | Crosswinds 0.1.0 |
|---|---|---|
| Static RAM | 57,904 B | 61,708 B |
| Flash, of the 6,553,600 B app slot | 5,577,051 B | 6,221,242 B |

Free heap on the Home screen was 128,980 B on a `default` build running on an X4. The
same figure for stock CrossPoint was not measured.

The game's largest tables (about 37 KB) are allocated when it opens and freed when
it closes, so the reader gets that memory back. If the allocation fails, the game
shows a message instead of opening. Heap use while the game is open has not been measured.

## Saves

Files live in `/.crosspoint/crosswinds` on the SD card, written through `HalStorage`.
A company left in flash LittleFS by the standalone firmware moves to the SD card on
the first start, and the flash copies are renamed `*.imported`.

## Updates

`OtaUpdater.cpp` reads `releases/latest` of this repository and looks for the asset
`crosswinds-<tag>-x3-x4.bin`. Tags are plain versions such as `0.1.0`, and
`platformio.ini` carries the same number.

## Staying close to upstream

The fork is based on CrossPoint tag 1.6.5. The changes outside the game are in
`HomeActivity`, `ActivityManager`, `OtaUpdater`, `HttpDownloader`, `platformio.ini`
and the GitHub workflows, so merges from upstream should touch few files.

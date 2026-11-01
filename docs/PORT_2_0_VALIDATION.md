# CrossPoint 2.0.0 port validation

Crosswinds development version: **2026.11.0**. Upstream base: CrossPoint
**2.0.0**, commit `42d58daaabcf469a2e5e1ba871714fb9dcf51d29`.
FreeInk SDK: `5e94fe7480f019c89fbf691aef87523eb1b46704`, as pinned by that release.

This is a local port candidate, not a published or hardware-tested release.
The published Crosswinds 2026.10.0 remains based on CrossPoint 1.6.5.

## Integration

The game remains an X4-only Home activity, after Settings. The optional
Plugins/OPDS entry uses CrossPoint's 2.0 menu logic. CrossPoint owns the activity
lifecycle, framebuffer, power, and SD access; the existing game bridge and game
sources are unchanged. Game memory is acquired on entry and released on exit.
Saves stay in `/.crosspoint/crosswinds`, including the existing one-time import
from standalone firmware's flash saves. Ships already at sea continue advancing
while the game is closed until the reader sleeps or restarts.

Updates still come from `terriblewitlogic/crosspoint-crosswinds`, with
`crosswinds-<version>-x3-x4.bin` assets and date-based version numbers. This keeps
the built-in game but means upstream releases must continue to be integrated
and tested in this fork. The partition layout is unchanged.

## Local verification — November 1, 2026

- `cmake -S test -B <build-dir> -DCMAKE_BUILD_TYPE=Release`, build, and CTest:
  **896 tests passed**, including the Crosswinds hold timer and upstream input,
  Bluetooth, plugin, release-parser, and reader checks.
- The game repository's `crosspoint-save-test` and `crosspoint-blit-test`, with
  `ROOT` pointing to this fork's `lib/Crosswinds/src`: **19 save/import/resume
  checks passed; 11,986 ink pixels compared with zero mismatches**.
- Repository `bin/clang-format-fix -g` with clang-format 21: passed.
- Release/CI workflow YAML parses, firmware/package versions agree, and the
  copied upstream README's relative links resolve.
- Four independent read-only reviews covered correctness, architecture,
  embedded constraints, and English strings/documentation. One documentation
  link finding was fixed. The LittleFS compile/link concern was resolved by
  the successful release firmware build.

Both `pio run -e gh_release` and `pio run -e default` passed. The development
build uses 70,524 bytes static RAM and 6,218,190 bytes reported flash; its binary
is 6,232,720 bytes. `pio check -e default --fail-on-defect low --fail-on-defect
medium --fail-on-defect high` passed with **no defects found**.

Release build measurements:

| Measurement | Bytes |
| --- | ---: |
| Static RAM (`.data` + `.bss`) | 70,500 |
| IRAM text including end padding | 69,632 |
| PlatformIO reported flash use | 6,168,186 |
| Actual `firmware.bin` | 6,182,720 |
| Application slot | 6,553,600 |
| Space remaining after binary | 370,880 |

These are build measurements, not runtime free-heap measurements. The release
build reports wolfSSL macro-redefinition warnings and two library dependency
cycles; it completes successfully. The local pioarduino core was restored from
6.2.0 to the repository/CI pin, 6.1.19, after the documented SCons failure. The
first build after the SDK rebuild could not find Bluetooth headers; retrying
with the rebuilt SDK completed without a firmware source change.

## Device verification still required

Back up the SD card's company folder and reading progress before testing.

1. On an X4, enter Crosswinds with Plugins disabled and enabled, and with/without
   an OPDS server and recent books. The final Home entry must open the game;
   returning from the game must select that entry.
2. Load an existing company, move a ship, quit through the Company Menu, and
   reopen. Progress must persist. Hold BACK, cancel, and confirm in separate
   attempts; the dismissing button must not trigger an extra game/Home action.
3. Sleep from the game and its quit dialog, then wake and reopen. The company
   must still load. If testing legacy flash import, use a backed-up device with
   an existing standalone company and verify it is imported only once.
4. Read a book with a Bluetooth page turner, return Home, enter/exit the game
   repeatedly, then resume reading. Remote input must remain confined to the
   reader. Record internal free heap and largest free block before/after each
   transition; investigate sustained loss, failed game allocation, or less
   than 50 KB free heap. The old 1.6.5 heap measurements are not port evidence.
5. Check reader-to-game-to-reader transitions in portrait, inverted portrait,
   landscape CW, and landscape CCW, including remapped reader controls. The
   game retains its fixed landscape physical-button layout. Check reading
   progress and cache regeneration for existing EPUB/TXT/Markdown books.
6. Check Settings' update screen still uses the Crosswinds release channel.
   Do not treat the old published 2026.10.0 as an upgrade for this candidate.

Human acceptance of the retained activity/OTA design and maintenance ownership,
and device test results, remain part of the firmware handoff.

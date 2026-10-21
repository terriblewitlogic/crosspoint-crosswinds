# Crosswinds for the Xteink X4

Crosswinds is a maritime trading game for the Xteink X4 e-reader. This repository is
a fork of [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader)
1.6.5 that adds the game as the last entry on the Home screen. Reading, the library
and the rest of the firmware are CrossPoint's. The differences are the Home entry, the
version number, the HTTP User-Agent, and where the update check looks (this
repository's releases).

If you would rather not keep a reader on the device, the same game is also available as
standalone X4 firmware: [terriblewitlogic/crosswinds](https://github.com/terriblewitlogic/crosswinds).

Version 2026.10.0 has been tested on one X4, flashed over USB and updated over Wi-Fi. It has
not been tried on an X3, X4 Pro or any other board. On an X3 the image hides the game. Expect rough edges.

## The game

You run a small fleet of ships in an invented sea, around 1810. Buy goods in one port,
sail them to another, sell where they are wanted. Prices move with stock and demand.
Sailing costs nothing, and cargo stays aboard until you sell it. Parcels are optional,
take one slot in the hold, and pay a fixed reward on delivery. Captains, a port-by-port
atlas and a recurring courier with seven story parcels fill out the rest. A new
company starts with a short guided voyage.

The game is played in landscape with the reader turned so the four front buttons run
down the right edge of the screen. From the top they act as UP, DOWN, OK and BACK.
The side rocker flips between ports and pages.

- **Quit:** hold BACK for a second or two and confirm, or choose QUIT in the Company Menu, which does not ask. Both save first.
- **Time:** in 2026.10.0 ships only move while the game is open. Builds from this repository after
  that keep ships already at sea sailing while the game is closed, until the reader sleeps or restarts.
- **Saves:** files in `/.crosspoint/crosswinds` on the SD card. Copy that folder to back up a company.
- **Input:** buttons only. There is no touch support, sound, or language other than English.

## Install

Flash `crosswinds-<version>-x3-x4.bin` from the
[releases page](https://github.com/terriblewitlogic/crosspoint-crosswinds/releases).

**Back up the device first.** A full copy of the flash lets you return to what you had:

```bash
pip install esptool
esptool.py --chip esp32c3 --port /dev/ttyACM0 --baud 921600 read_flash 0x0 0x1000000 backup.bin
```

Then write the firmware at offset `0x10000`, as CrossPoint documents for its own releases:

```bash
esptool.py --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 crosswinds-2026.10.0-x3-x4.bin
```

I tested the `0.1.0rc` file from GitHub with the command above, on an X4 that already
ran this fork, and wrote the 2026.10.0 file the same way afterwards. The release candidate
has the same code as this release and a different version number. Flashing over stock
Xteink firmware or an unmodified CrossPoint, and CrossPoint's
[web installer](https://crosspointreader.com/#flash-tools) (custom `.bin`), have not been
tried. (Newer esptool versions print a deprecation warning for `write_flash`; it still works.)

**If the device has updated itself before**, from Settings or from the SD card, it may boot
from the second of its two app slots (each update switches to the other one), and the
command above writes the first. The old firmware would keep running. Clear the boot
record first, which makes the device use the first slot again, and then write as above:

```bash
esptool.py --chip esp32c3 --port /dev/ttyACM0 --baud 921600 erase_region 0xe000 0x2000
```

I checked this on an X4 that had updated itself: after the erase it came up in the first
slot, and the file written there then ran. On a device that has never updated the first
slot is already the one it boots, so the erase should change nothing; that case was not tried.

If your X4 came from a third-party store with USB flashing locked, do not use
CrossPoint's unlock tool for this firmware. It only supports CrossPoint and CrossInk,
and flashing anything else on a locked device can leave it stuck. See
[docs/CROSSPOINT_README.md](docs/CROSSPOINT_README.md#usb-locked-devices-xteink-unlocker).

After the first flash, Settings → System → Check for Updates reads this repository's
releases. I used it once, to go from 0.1.0rc to 2026.10.0 on one X4. It needs a working
2.4 GHz connection: the X4's radio has no 5 GHz, and over a weak phone hotspot the
download did not finish. Settings → System → SD Card Firmware Update installs a `.bin`
from the card without Wi-Fi; I have not tried that one.

## Going back

- **To CrossPoint:** flash an official release. Your books and settings stay on the SD
  card, and the Crosswinds folder is left in place unused.
- **To what you had before:** write the backup, `write_flash 0x0 backup.bin`. This has
  not been tested here.

## Build from source

```bash
git clone --recurse-submodules https://github.com/terriblewitlogic/crosspoint-crosswinds.git
cd crosspoint-crosswinds
pio run -e default                  # build
pio run -e default -t upload        # build and flash over USB
```

Use the PlatformIO core that CI pins (pioarduino 6.1.19, see `.github/workflows/ci.yml`);
stock PlatformIO 6.2.0 failed here with a SCons error.

The game is in `lib/Crosswinds`. It is maintained in a private repository and copied here by
`scripts/sync_crosswinds.sh`, so edits to that folder can be overwritten. The standalone
firmware linked above is built from the same game code. Bug reports for it are welcome here. How the port works is in
[docs/CROSSWINDS.md](docs/CROSSWINDS.md). The upstream CrossPoint README is kept at
[docs/CROSSPOINT_README.md](docs/CROSSPOINT_README.md).

## How it was made

Most of the code and in-game text was written with Claude Code and Codex, and the artwork was
generated with OpenAI's image tool and reduced to 1-bit. The maintainer directed the
work and tested it on a device. [NOTICE](NOTICE) has the details and the other credits.

## Licence and credits

MIT, as CrossPoint is. See [LICENSE](LICENSE) and [NOTICE](NOTICE).

This is not an official CrossPoint release and is not affiliated with the CrossPoint
Reader organization or Xteink. Report problems with Crosswinds in this repository,
not upstream.

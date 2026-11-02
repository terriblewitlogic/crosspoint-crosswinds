# Crosswinds touch test

Requires Python 3.12+, SDL2, pkg-config, a C++ compiler, LovyanGFX 1.2.24 and a
checkout of `terriblewitlogic/crosswinds` containing its SDL harness. Tested with
game harness revision `8acbda2c5decaafd916398b1fdaca55edcd3b9d7`.

```sh
python3 test/crosswinds_touch/run.py /path/to/crosswinds --lgfx /path/to/LovyanGFX
python3 test/crosswinds_touch/run.py /path/to/crosswinds --lgfx /path/to/LovyanGFX --buttons
```

The runner exports the tracked harness to a temporary directory and overlays this
fork's game sources. It does not alter either checkout. The touch test uses the
same tap/swipe entry points as the Pro activity and verifies the first trading
voyage and save reload. The button run checks the existing X4 onboarding path.

This tests game input and rendering, not the SDK's touch hardware, coordinate
calibration or CrossPoint activity lifecycle. Those require an X4 Pro.

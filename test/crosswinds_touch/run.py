#!/usr/bin/env python3
"""Build the fork's game in the standalone SDL harness without editing either checkout."""

import argparse
import io
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("game_checkout", type=Path, help="standalone crosswinds checkout with tools/host")
parser.add_argument("--lgfx", type=Path, required=True, help="LovyanGFX 1.2.24 checkout")
parser.add_argument("--buttons", action="store_true", help="run the original button onboarding regression")
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
archive = subprocess.check_output([
    "git", "-C", str(args.game_checkout.resolve()), "archive", "HEAD",
    "*.cpp", "*.h", "*.ino", "tools/host", "tools/headless",
])
with tempfile.TemporaryDirectory(prefix="crosswinds-touch-") as scratch:
    work = Path(scratch)
    with tarfile.open(fileobj=io.BytesIO(archive)) as source:
        source.extractall(work, filter="data")
    for source in (repo / "lib/Crosswinds/src").iterdir():
        if source.suffix in (".cpp", ".h"):
            shutil.copy2(source, work / source.name)
    host = work / "tools/host"
    main = host / "main.cpp"
    text = main.read_text()
    if not args.buttons:
        include = '#include "x4_onboarding_test.h"'
        dispatch = '  if (getenv("SH_X4_ONBOARDING_TEST"))'
        if include not in text or dispatch not in text:
            raise RuntimeError("The standalone SDL harness has changed; update the test insertion points.")
        shutil.copy2(repo / "test/crosswinds_touch/TouchJourney.h", host / "TouchJourney.h")
        text = text.replace(include, include + '\n#include "TouchJourney.h"', 1)
        text = text.replace(dispatch,
                            '  if (getenv("SH_TOUCH_TEST")) host_test_exit(run_crosswinds_touch_test() ? 0 : 1);\n'
                            + dispatch, 1)
        main.write_text(text)
    flags = "-DSH_X4_VOYAGE_STANDBY=1 -DSH_X4_STANDBY_DIAGNOSTICS=1"
    if not args.buttons:
        flags += " -DSH_TOUCH_UI=1"
    subprocess.run([
        "make", "-C", str(host), "-j4", "build_x4/ships_host_x4",
        f"LGFX={args.lgfx.resolve()}", f"X4_FEATURE_FLAGS={flags}",
    ], check=True)
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SH_FS_DIR=str(work / "save"))
    env["SH_X4_ONBOARDING_TEST" if args.buttons else "SH_TOUCH_TEST"] = "1"
    subprocess.run([str(host / "build_x4/ships_host_x4")], env=env, check=True)

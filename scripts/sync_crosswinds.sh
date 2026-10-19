#!/bin/sh
# Copy the Crosswinds game sources into lib/Crosswinds/src.
# Usage: scripts/sync_crosswinds.sh /path/to/crosswinds-checkout
# The game repository is the source of truth; this copy is what the firmware
# builds. ships.ino and board_x4.cpp are the standalone firmware's boot and
# hardware layer, and the other renderers belong to other devices, so they
# stay behind: crosspoint_main.cpp and src/activities/crosswinds replace them.
set -eu
SRC=${1:?usage: $0 /path/to/crosswinds-checkout}
DST=$(cd "$(dirname "$0")/.." && pwd)/lib/Crosswinds/src
rm -rf "$DST"
mkdir -p "$DST"
for f in "$SRC"/*.cpp "$SRC"/*.h; do
  case "$(basename "$f")" in
    board_x4.cpp|render.cpp|render_hd.cpp|render_eink.cpp) continue ;;
  esac
  cp "$f" "$DST/"
done
echo "Copied $(ls "$DST" | wc -l | tr -d ' ') files from $SRC ($(git -C "$SRC" rev-parse --short HEAD 2>/dev/null || echo unknown))"

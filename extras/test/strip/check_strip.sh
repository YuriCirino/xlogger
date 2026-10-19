#!/usr/bin/env bash
# Proves on a real toolchain that levels above XLOGGER_MAX_LEVEL leave no
# strings in the flashed firmware, for both logger.xxx() methods and XLOG_*
# macros. Needs PlatformIO Core and pyelftools.
#   bash extras/test/strip/check_strip.sh <board>
set -euo pipefail
board="$1"
here="$(cd "$(dirname "$0")" && pwd)"
work="$(mktemp -d)"
fail=0

build() {  # build with the given max level, print "MARKER section flash|debug" lines
  local level="$1"
  local dir="$work/l$level"
  pio ci --lib="." --board="$board" --keep-build-dir --build-dir="$dir" \
    --project-option="build_flags=-D XLOGGER_MAX_LEVEL=$level" \
    extras/test/strip/strip.ino > "$dir.log" 2>&1 || { cat "$dir.log"; exit 1; }
  python3 "$here/find_markers.py" "$dir/.pio/build/$board/firmware.elf"
}

all="$(build 4)"
release="$(build 2)"
echo "== $board, XLOGGER_MAX_LEVEL=4 (marker, ELF section, flash|debug):"; echo "$all"
echo "== $board, XLOGGER_MAX_LEVEL=2:"; echo "$release"

in_flash() { echo "$1" | awk -F'\t' '$3=="flash"{print $1}' | sort -u; }

# Sanity: with every level on, all six markers must be in flash (detection works).
[ "$(in_flash "$all" | grep -c MARKER)" -eq 6 ] || { echo "FAIL expected 6 markers in flash at level 4"; fail=1; }
# Release: debug/info gone from flash, warn/error kept.
if in_flash "$release" | grep -q STRIP_MARKER; then echo "FAIL disabled strings still in flash"; fail=1; fi
[ "$(in_flash "$release" | grep -c KEEP_MARKER)" -eq 2 ] || { echo "FAIL enabled strings missing"; fail=1; }

[ "$fail" -eq 0 ] && echo "ok   $board: disabled levels removed from the flashed firmware"
exit "$fail"

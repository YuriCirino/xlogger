#!/usr/bin/env bash
# Proves on a real toolchain that levels above XLOGGER_MAX_LEVEL leave no
# strings in the firmware, for both logger.xxx() methods and XLOG_* macros.
#   bash extras/test/strip/check_strip.sh <board>      (needs PlatformIO Core)
set -euo pipefail
board="$1"
work="$(mktemp -d)"
fail=0

markers() {  # markers found in the ELF built with the given max level
  local level="$1" dir="$work/l$level"
  pio ci --lib="." --board="$board" --keep-build-dir --build-dir="$dir" \
    --project-option="build_flags=-D XLOGGER_MAX_LEVEL=$level" \
    extras/test/strip/strip.ino > "$dir.log" 2>&1 || { cat "$dir.log"; exit 1; }
  strings -a "$dir/.pio/build/$board/firmware.elf" \
    | grep -oE '(STRIP|KEEP)_MARKER_[A-Z_]+' | sort -u
}

all="$(markers 4)"
release="$(markers 2)"
echo "== $board, XLOGGER_MAX_LEVEL=4:"; echo "$all"
echo "== $board, XLOGGER_MAX_LEVEL=2:"; echo "$release"

# Sanity: with every level on, all six markers must be found (proves detection works).
[ "$(echo "$all" | grep -c MARKER)" -eq 6 ] || { echo "FAIL expected 6 markers at level 4"; fail=1; }
# Release: debug/info gone, warn/error kept.
if echo "$release" | grep -q STRIP_MARKER; then echo "FAIL disabled strings still in firmware"; fail=1; fi
[ "$(echo "$release" | grep -c KEEP_MARKER)" -eq 2 ] || { echo "FAIL enabled strings missing"; fail=1; }

[ "$fail" -eq 0 ] && echo "ok   $board: disabled levels removed from firmware"
exit "$fail"

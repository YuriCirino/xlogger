#!/usr/bin/env bash
# Proves on a real toolchain which disabled log calls leave their strings in
# the flashed firmware. Needs PlatformIO Core and pyelftools.
#
#   bash extras/test/strip/check_strip.sh <board> [extra build flags] [expect]
#
#   expect = all     (default) every disabled string is gone, methods and macros
#            macros  only XLOG_* macro strings are guaranteed gone; method strings
#                    may remain (toolchains that build the sketch unoptimized)
set -euo pipefail
board="$1"
extra="${2:-}"
expect="${3:-all}"
here="$(cd "$(dirname "$0")" && pwd)"
work="$(mktemp -d)"
fail=0

build() {  # build with the given max level, print "MARKER<TAB>section<TAB>flash|debug"
  local level="$1"
  local dir="$work/l$level"
  pio ci -v --lib="." --board="$board" --keep-build-dir --build-dir="$dir" \
    --project-option="build_flags=-D XLOGGER_MAX_LEVEL=$level $extra" \
    extras/test/strip/strip.ino > "$dir.log" 2>&1 || { cat "$dir.log"; exit 1; }
  { grep -E 'strip\.ino' "$dir.log" | grep -m1 -E -- ' -c ' || true; } | tr ' ' '\n' \
    | grep -xE -- '-(O[0-9sgz]?|g[0-9a-z]*|f(no-)?inline[a-z-]*|flto.*)' | sort -u | tr '\n' ' ' \
    | sed "s/^/compiler flags for the sketch: /" >&2 || true
  echo >&2
  python3 "$here/find_markers.py" "$dir/.pio/build/$board/firmware.elf"
}

echo "== board=$board extra='$extra' expect=$expect"
all="$(build 4)"
release="$(build 2)"
echo "== XLOGGER_MAX_LEVEL=4 (marker, ELF section, flash|debug):"; echo "$all"
echo "== XLOGGER_MAX_LEVEL=2:"; echo "$release"

in_flash() { echo "$1" | awk -F'\t' '$3=="flash"{print $1}' | sort -u; }

# Sanity: with every level on, all six markers must be in flash (detection works).
[ "$(in_flash "$all" | grep -c MARKER)" -eq 6 ] || { echo "FAIL expected 6 markers in flash at level 4"; fail=1; }
# Enabled levels must always stay.
[ "$(in_flash "$release" | grep -c KEEP_MARKER)" -eq 2 ] || { echo "FAIL enabled strings missing"; fail=1; }
# Disabled macros must always be gone.
if in_flash "$release" | grep -q STRIP_MARKER_MACRO; then echo "FAIL disabled macro strings still in flash"; fail=1; fi
# Disabled methods must be gone when expected.
if in_flash "$release" | grep -q STRIP_MARKER_METHOD; then
  if [ "$expect" = all ]; then echo "FAIL disabled method strings still in flash"; fail=1
  else echo "note: disabled method strings remain in flash on $board with these flags (expected)"; fi
fi

[ "$fail" -eq 0 ] && echo "ok   $board ($expect): check passed"
exit "$fail"

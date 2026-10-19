#!/usr/bin/env sh
# Builds and runs every host test. Run from the repository root:
#   sh extras/test/run.sh
set -e
out="${TMPDIR:-/tmp}/xlogger-tests"
mkdir -p "$out"
for t in test_xlogger test_max_level; do
  g++ -std=gnu++11 -Wall -Wextra -Werror -I extras/test -I src \
      "extras/test/$t.cpp" -o "$out/$t"
  "$out/$t"
done

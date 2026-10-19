#!/usr/bin/env bash
# Runs a command; if it fails, re-emits the last 60 lines of its output as a
# single GitHub Actions error annotation, so the cause shows on the run page.
#   bash extras/tools/annotate_on_fail.sh <command> [args...]
log="$(mktemp)"
"$@" 2>&1 | tee "$log"
status=${PIPESTATUS[0]}
if [ "$status" -ne 0 ]; then
  msg="$(tail -n 60 "$log" | sed -e 's/%/%25/g' | awk 'BEGIN{ORS="%0A"} {print}')"
  echo "::error title=Command failed: $1::$msg"
fi
exit "$status"

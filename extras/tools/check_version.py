#!/usr/bin/env python3
"""Check that the version is the same everywhere.

  python3 extras/tools/check_version.py          # all files agree
  python3 extras/tools/check_version.py v1.1.0   # ...and match this git tag
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def grab(path, pattern):
    m = re.search(pattern, (ROOT / path).read_text(encoding="utf-8"), re.M)
    return m.group(1) if m else None


versions = {
    "library.json": json.loads((ROOT / "library.json").read_text())["version"],
    "library.properties": grab("library.properties", r"^version=(\S+)$"),
    "src/xLogger.h": grab("src/xLogger.h", r'#define XLOGGER_VERSION "([^"]+)"'),
    "skills/xlogger/SKILL.md": grab("skills/xlogger/SKILL.md", r'^\s+version:\s*"([^"]+)"'),
}

for name, v in versions.items():
    print(f"{v or 'MISSING':>10}  {name}")

failed = len(set(versions.values())) != 1 or None in versions.values()
if failed:
    print("FAIL versions differ")

if len(sys.argv) > 1:
    tag = sys.argv[1].removeprefix("refs/tags/")
    expected = "v" + versions["library.json"]
    if tag != expected:
        print(f"FAIL tag {tag} does not match version {expected}")
        failed = True
    else:
        print(f"ok   tag {tag} matches")

sys.exit(1 if failed else 0)

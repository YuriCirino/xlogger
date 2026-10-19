#!/usr/bin/env python3
"""Keep the AI-facing documentation correct.

ai/skills/xlogger/SKILL.md is the single source of truth. This script:

  build  - regenerates ai/llms-full.txt from SKILL.md
  check  - fails if llms-full.txt is out of date, or if any ```cpp block in
           SKILL.md does not compile against the host mock (extras/test).
           Blocks fenced as ```c++ are fragments/anti-patterns and are skipped.

Run from the repository root:
  python3 extras/tools/ai_docs.py build
  python3 extras/tools/ai_docs.py check
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SKILL = ROOT / "ai" / "skills" / "xlogger" / "SKILL.md"
FULL = ROOT / "ai" / "llms-full.txt"
HEADER_FILE = ROOT / "src" / "xLogger.h"


def read_skill():
    text = SKILL.read_text(encoding="utf-8")
    m = re.match(r"^---\n(.*?)\n---\n", text, re.S)
    if not m:
        sys.exit(f"{SKILL}: missing YAML frontmatter")
    return m.group(1), text[m.end():].lstrip("\n")


def lib_version():
    m = re.search(r'#define XLOGGER_VERSION "([^"]+)"', HEADER_FILE.read_text())
    return m.group(1)


def render_full():
    frontmatter, body = read_skill()
    skill_version = re.search(r'version:\s*"([^"]+)"', frontmatter).group(1)
    if skill_version != lib_version():
        sys.exit(f"SKILL.md version {skill_version} != XLOGGER_VERSION {lib_version()}")
    header = (
        f"<!-- xLogger {lib_version()} - complete reference for AI coding assistants.\n"
        "     Attach this file to your chat, or point your AI to its URL, when asking\n"
        "     for Arduino/PlatformIO code that uses xLogger.\n"
        "     GENERATED from ai/skills/xlogger/SKILL.md - do not edit by hand. -->\n\n"
    )
    return header + body


def check_snippets():
    _, body = read_skill()
    blocks = re.findall(r"^```cpp\n(.*?)^```", body, re.S | re.M)
    if not blocks:
        sys.exit("no ```cpp blocks found in SKILL.md")
    failed = 0
    with tempfile.TemporaryDirectory() as tmp:
        for i, code in enumerate(blocks, 1):
            src = Path(tmp) / f"snippet_{i}.cpp"
            src.write_text(code)
            r = subprocess.run(
                ["g++", "-std=gnu++11", "-fsyntax-only", "-Wall", "-Werror",
                 "-include", "Arduino.h",
                 "-I", str(ROOT / "extras" / "test"), "-I", str(ROOT / "src"), str(src)],
                capture_output=True, text=True)
            first = code.strip().splitlines()[0][:60]
            if r.returncode:
                failed += 1
                print(f"FAIL snippet {i} ({first})\n{r.stderr}")
            else:
                print(f"ok   snippet {i} ({first})")
    return failed


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "check"
    if cmd == "build":
        FULL.write_text(render_full(), encoding="utf-8")
        print(f"wrote {FULL.relative_to(ROOT)}")
    elif cmd == "check":
        failed = check_snippets()
        if FULL.read_text(encoding="utf-8") != render_full():
            print("FAIL llms-full.txt is out of date: run `python3 extras/tools/ai_docs.py build`")
            failed += 1
        else:
            print("ok   llms-full.txt is up to date")
        sys.exit(1 if failed else 0)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()

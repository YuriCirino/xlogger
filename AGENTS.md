# AGENTS.md

Instructions for AI coding agents working **on this repository**. If you are writing
firmware that *uses* xLogger, read [`skills/xlogger/SKILL.md`](skills/xlogger/SKILL.md) instead.

## Layout

- `src/xLogger.h` — the whole library (header-only).
- `examples/` — Arduino sketches, built in CI for uno, leonardo, esp32dev, nodemcuv2.
- `extras/test/` — host unit tests with a mock `Arduino.h`.
- `extras/tools/ai_docs.py` — builds `llms-full.txt` and compile-checks AI doc snippets.
- `skills/xlogger/SKILL.md` — source of truth for AI-facing docs; `llms-full.txt` is generated from it.
- `docs/GUIA-pt-BR.md` — Portuguese guide.

## Commands

```bash
# unit tests
g++ -std=gnu++11 -Wall -Wextra -Werror -I extras/test -I src \
    extras/test/test_xlogger.cpp -o extras/test/test_xlogger && ./extras/test/test_xlogger

# AI docs: regenerate, then verify snippets compile and llms-full.txt is in sync
python3 extras/tools/ai_docs.py build
python3 extras/tools/ai_docs.py check

# build an example for a real board (needs PlatformIO Core and network)
pio ci --lib="." --board=uno examples/Basic/Basic.ino
```

## Constraints

- C++11 only (AVR toolchain): no `std::` containers, exceptions, RTTI, C++14/17 features.
- No heap allocation and no `String` in the logging path.
- A compiled-out `XLOG_*` macro must expand to `((void)0)` and not evaluate its arguments.
- Do not break the public API in a minor release (Semantic Versioning).
- When the public API changes, update **all** of: `README.md`, `docs/GUIA-pt-BR.md`,
  `skills/xlogger/SKILL.md` (then `ai_docs.py build`), `CHANGELOG.md` (under `Unreleased`),
  and add a test in `extras/test/test_xlogger.cpp`.
- Version lives in four places that must match: `library.json`, `library.properties`,
  `XLOGGER_VERSION` in `src/xLogger.h`, and `metadata.version` in `SKILL.md`.
- Never edit `llms-full.txt` by hand.
- Style: 2-space indent, `camelCase` methods, trailing `_` on private members.

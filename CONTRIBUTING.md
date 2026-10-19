# Contributing to xLogger

Thanks for your interest! Bug reports, ideas, documentation fixes and code are all welcome.
English or Portuguese are both fine everywhere in this project.

> 🇧🇷 Pode contribuir em português: issues, pull requests e comentários.

## Reporting bugs and suggesting features

Use the [issue forms](https://github.com/YuriCirino/xlogger/issues/new/choose):

- **🐛 Bug report** — include the board, xLogger version, a *minimal* sketch that reproduces
  the problem and the Serial Monitor output or compiler error.
- **💡 Feature request** — describe the problem first, then the API you imagine.
- **❓ Question** — for usage doubts not answered by the docs.

Search the [existing issues](https://github.com/YuriCirino/xlogger/issues) before opening a new one.

## Contributing code

1. **Open an issue first** for anything bigger than a small fix, so we can agree on the
   approach before you spend time on it.
2. Fork the repository and create a branch from `main`:
   `git checkout -b fix/short-description` or `feat/short-description`.
3. Make your change, keeping it focused on one thing.
4. Run the tests (below) and make sure they pass.
5. Open a pull request against `main` and fill in the template. Link the issue
   (`Closes #12`).

CI builds every example for Uno, Leonardo, ESP32 and ESP8266 and runs the unit tests on
each pull request.

### Running the tests

The unit tests run on your computer with a mock of the Arduino `Print` class, no board needed.
From the repository root:

```bash
sh extras/test/run.sh
```

`test_xlogger.cpp` runs with every level compiled in; `test_max_level.cpp` checks the
compile-time removal. CI also builds `extras/test/strip/strip.ino` for real boards and
checks that disabled strings are absent from the firmware.

To build the examples for real boards locally (requires [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html)):

```bash
pio ci --lib="." --board=uno --board=esp32dev examples/Basic/Basic.ino
```

### Code guidelines

- **Keep it header-only and dependency-free.** Everything lives in `src/xLogger.h`.
- **C++11.** The AVR toolchain (Arduino Uno) is the lowest common denominator: no
  `std::` containers, no exceptions, no RTTI, no C++14/17 features.
- **No heap allocation** and no `String` in the logging path.
- **Disabled levels must cost nothing.** New `XLOG_*` macros must expand to `((void)0)`
  when compiled out and must not evaluate their arguments.
- **Don't break the public API** in a minor release (see [Semantic Versioning](https://semver.org/)).
- Follow the existing style: 2-space indent, `camelCase` methods, trailing `_` on private members.
- Add a test in `extras/test/` for new behavior.
- Update `README.md` **and** `docs/GUIA-pt-BR.md` when the public API changes
  (if you don't write Portuguese, say so in the PR and the maintainer will translate).
- Update the AI reference in `skills/xlogger/SKILL.md` too, then regenerate and check it:
  `python3 extras/tools/ai_docs.py build && python3 extras/tools/ai_docs.py check`.
  Code blocks fenced as ` ```cpp ` must be complete, compilable sketches (CI checks them);
  use ` ```c++ ` for fragments and anti-patterns. Never edit `llms-full.txt` by hand.
- Add a line under `## [Unreleased]` in `CHANGELOG.md`.

### Commit messages

Short imperative summary (≤ 72 characters), e.g. `Add color output option` or
`Fix timestamp on ESP8266`, followed by a blank line and details if needed.

## Code of Conduct

This project follows the [Contributor Covenant](CODE_OF_CONDUCT.md). By participating,
you agree to uphold it.

## License

By contributing, you agree that your contributions will be licensed under the
[MIT License](LICENSE).

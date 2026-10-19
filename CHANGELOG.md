# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project
uses [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [1.2.0] - 2026-10-19

### Changed
- `logger.error/warn/info/debug()` now honor `XLOGGER_MAX_LEVEL`: above it their code is
  removed from the firmware, like the `XLOG_*` macros, and on optimized builds (AVR) their
  text too. On ESP32 with PlatformIO's default (unoptimized) sketch build the text stays.
  Arguments are still evaluated, so debug and release builds behave the same.
  `here()`, `enabled()` and the legacy `print/println` honor it too.
- Documentation recommends `logger.info()` (Serial.print-like) as the default style;
  macros are presented as the advanced option. README and the Portuguese guide start
  with a table of contents and a "two ways to log" comparison.

### Added
- CI builds a marker sketch for Uno and ESP32 at two levels and checks the firmware for
  the disabled strings.

## [1.1.0] - 2026-10-19

No changes to the library code; everything from 1.0.0 keeps working.

### Added
- `llms-full.txt` and the Agent Skill now ship with the library, so they are available
  inside `.pio/libdeps/` and in the Arduino libraries folder.
- CI: `arduino-lint` (Arduino Library Manager rules) and a check that the version matches
  in every file. Releases are published to the PlatformIO Registry automatically.
- Issue forms (bug report, feature request, question), pull request template.
- `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md` and `SECURITY.md`.
- Clearer link to the Portuguese guide; feedback section in README and guide.
- AI assistant support: Agent Skill (`skills/xlogger/SKILL.md`), generated `llms-full.txt`,
  `llms.txt` index and `AGENTS.md`. CI compiles every snippet of the AI reference.

### Fixed
- Examples no longer hang on native-USB boards running without a PC
  (`while (!Serial)` now has a 3 s timeout).

## [1.0.0] - 2026-10-19

First public release.

- Levels `error`, `warn`, `info`, `debug` with runtime filtering.
- `XLOG_*` macros with compile-time removal via `XLOGGER_MAX_LEVEL`.
- Variadic printing forwarded to `Print::print()` (no `String`, `F()` support).
- `XLOG_HERE` with caller function and line.
- Optional `millis()` timestamp and per-logger tag.
- Works with any `Print`.
- Compatibility API (`setDebugMode`, `print`, `println`) from the original `Logger` class.

[Unreleased]: https://github.com/YuriCirino/xlogger/compare/v1.2.0...HEAD
[1.2.0]: https://github.com/YuriCirino/xlogger/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/YuriCirino/xlogger/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/YuriCirino/xlogger/releases/tag/v1.0.0

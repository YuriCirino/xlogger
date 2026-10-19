# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project
uses [Semantic Versioning](https://semver.org/).

## [Unreleased]

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

[Unreleased]: https://github.com/YuriCirino/xlogger/compare/v1.1.0...HEAD
[1.1.0]: https://github.com/YuriCirino/xlogger/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/YuriCirino/xlogger/releases/tag/v1.0.0

# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project
uses [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added
- Issue forms (bug report, feature request, question), pull request template.
- `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md` and `SECURITY.md`.
- Clearer link to the Portuguese guide; feedback section in README and guide.

## [1.0.0] - 2026-10-19

First public release.

- Levels `error`, `warn`, `info`, `debug` with runtime filtering.
- `XLOG_*` macros with compile-time removal via `XLOGGER_MAX_LEVEL`.
- Variadic printing forwarded to `Print::print()` (no `String`, `F()` support).
- `XLOG_HERE` with caller function and line.
- Optional `millis()` timestamp and per-logger tag.
- Works with any `Print`.
- Compatibility API (`setDebugMode`, `print`, `println`) from the original `Logger` class.

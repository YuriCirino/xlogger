# Changelog

## [1.0.0] - 2026-10-19

First public release.

- Levels `error`, `warn`, `info`, `debug` with runtime filtering.
- `XLOG_*` macros with compile-time removal via `XLOGGER_MAX_LEVEL`.
- Variadic printing forwarded to `Print::print()` (no `String`, `F()` support).
- `XLOG_HERE` with caller function and line.
- Optional `millis()` timestamp and per-logger tag.
- Works with any `Print`.
- Compatibility API (`setDebugMode`, `print`, `println`) from the original `Logger` class.

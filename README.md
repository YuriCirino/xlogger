# xLogger

[![CI](https://github.com/YuriCirino/xlogger/actions/workflows/ci.yml/badge.svg)](https://github.com/YuriCirino/xlogger/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Debug logging for Arduino and PlatformIO that works like `Serial.print()`, but can be
turned off without commenting lines and removed entirely from the final firmware.

**English** · [Português](docs/GUIA-pt-BR.md)

> [!NOTE]
> 🇧🇷 **Fala português?** Existe uma página separada com o guia completo, passo a passo, em português:
> **[clique aqui para abrir o guia em português](docs/GUIA-pt-BR.md)**.

## Contents

| | |
|---|---|
| 🚀 [Quick start](#quick-start) | Install and print your first log in 1 minute |
| ⚖️ [Two ways to log](#two-ways-to-log-loggerinfo-or-xlog_info) | `logger.info()` vs `XLOG_INFO()` — which one to use |
| 📶 [Levels](#levels) | error, warn, info, debug |
| 🔧 [Debug and release builds](#debug-and-release-builds) | Remove logs from the final firmware |
| 📖 [API reference](#api-reference) | Every method and option |
| 💡 [Examples](#examples) | Ready-to-run sketches |
| 🤖 [Using with AI assistants](#using-with-ai-assistants) | Make ChatGPT, Claude, Copilot… write correct xLogger code |
| 🐛 [Feedback and bugs](#feedback-bugs-and-contributing) | Report a problem or suggest an idea |

## Quick start

**1. Install** — in `platformio.ini`:

```ini
lib_deps =
    https://github.com/YuriCirino/xlogger.git#v1.2.0
```

Arduino IDE: *Code → Download ZIP* on this page, then *Sketch → Include Library → Add .ZIP Library…*

**2. Log** — if you know `Serial.print()`, you already know xLogger:

```cpp
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  logger.info("boot ok");
}

void loop() {
  int adc = analogRead(A0);
  logger.debug("adc=", adc, " volts=", adc * 5.0 / 1023);
  delay(1000);
}
```

```text
[I] boot ok
[D] adc=512 volts=2.50
```

Separate values with **commas** — no `String()` and no `+`. To silence the debug lines,
add one line: `logger.setLevel(XLogLevel::Info);`

## Two ways to log: `logger.info()` or `XLOG_INFO()`

Both print exactly the same thing. **If in doubt, use `logger.info()`.**

| | `logger.info(...)` ✅ recommended | `XLOG_INFO(logger, ...)` advanced |
|---|---|---|
| Looks like | `Serial.print()` | A C macro |
| Turned off at runtime with `setLevel()` | ✅ | ✅ |
| Removed from the firmware in release builds | ✅ Code always; text too on AVR (Uno, Nano…)¹ | ✅ Code and text, on every board |
| Arguments run in release builds | ✅ Yes — the program behaves the same in debug and release | ❌ No — skipped entirely, so **no side effects** inside |
| Best for | Everyday logging | Expensive arguments (a function that takes time to compute) |

¹ On ESP32 with PlatformIO's default settings the sketch is compiled without optimization,
so the text of disabled method calls stays in flash (a few bytes per call; the ESP32 has
megabytes of flash). To remove it too, add `-Os` to `build_flags` — verified in CI:
`build_flags = -Os -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN`. The macros remove it either way.

```cpp
#include <xLogger.h>

XLogger logger(Serial);

int readSensor() { return analogRead(A0); }  // takes time and must always run

void setup() { Serial.begin(115200); }

void loop() {
  logger.debug("sensor=", readSensor());        // readSensor() always runs
  XLOG_DEBUG(logger, "sensor=", readSensor());  // in release, readSensor() is NOT called
  XLOG_HERE(logger);                            // macro only: prints [H] loop:12
  delay(1000);
}
```

`XLOG_HERE(logger)`, which prints the current function and line number, only exists as a macro.

## Features

- **Four levels** — `error`, `warn`, `info`, `debug` — change them at runtime with `setLevel()`.
- **Removed from release builds** — `-D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN` removes debug/info
  calls from the firmware.
- **No `String`** — values go straight to `Print::print()`: no heap allocation, no fragmentation.
- **Any output** — `Serial`, `Serial1`, USB (Leonardo, ESP32-S3, RP2040), `SoftwareSerial`,
  an LCD, an SD `File`… anything that is a `Print`.
- **Timestamps and tags** — `1520 [D][IMU] ...`; one logger per module.
- **Header-only**, no dependencies, C++11 — tested on AVR, ESP32 and ESP8266.

## Levels

| Method | Macro | Prints | Use for |
|---|---|---|---|
| `logger.error(...)` | `XLOG_ERROR(logger, ...)` | `[E] ...` | Something failed |
| `logger.warn(...)`  | `XLOG_WARN(logger, ...)`  | `[W] ...` | Abnormal but recoverable |
| `logger.info(...)`  | `XLOG_INFO(logger, ...)`  | `[I] ...` | Important events: boot, connected, state change |
| `logger.debug(...)` | `XLOG_DEBUG(logger, ...)` | `[D] ...` | Variable values while developing |
| — | `XLOG_HERE(logger)` | `[H] loop:42` | "Reached this point" |

A line prints when its level is at or below the logger's level:

```cpp
logger.setLevel(XLogLevel::Warn);   // only [W] and [E]
logger.setLevel(XLogLevel::Debug);  // everything (default)
logger.setLevel(XLogLevel::None);   // nothing
```

Tip: wrap text in `F("...")` — `logger.info(F("boot ok"))` — to keep it in flash instead of
RAM. It matters on boards with little memory like the Uno (2 KB RAM).

## Debug and release builds

`setLevel()` hides messages, but their code and text stay in the firmware. To **remove**
them, set the highest level compiled in, in `platformio.ini`:

```ini
[env:debug]
build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_DEBUG   ; everything

[env:release]
build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN    ; only warnings and errors
```

Values: `XLOG_LEVEL_NONE` (0), `XLOG_LEVEL_ERROR` (1), `XLOG_LEVEL_WARN` (2),
`XLOG_LEVEL_INFO` (3), `XLOG_LEVEL_DEBUG` (4, default). This works for both
`logger.debug()` and `XLOG_DEBUG()`; CI checks it by inspecting real Uno and ESP32 firmware
(see the [ESP32 note](#two-ways-to-log-loggerinfo-or-xlog_info) about method text).

> ⚠️ With macros (`XLOG_DEBUG`), the arguments disappear too: `XLOG_DEBUG(logger, i++)` does
> not increment `i` in a release build. Methods (`logger.debug`) don't have this pitfall.

Without PlatformIO, `#define XLOGGER_MAX_LEVEL XLOG_LEVEL_WARN` **before**
`#include <xLogger.h>` works for that file.

## API reference

| Member | Description |
|---|---|
| `XLogger logger(Serial)` | Create a logger. Optional 2nd argument: starting level, e.g. `XLogLevel::Info` |
| `logger.error/warn/info/debug(values...)` | Print a line with `[E]`, `[W]`, `[I]` or `[D]` |
| `XLOG_ERROR/WARN/INFO/DEBUG(logger, values...)` | Same, as macros (see [Two ways to log](#two-ways-to-log-loggerinfo-or-xlog_info)) |
| `XLOG_HERE(logger)` | Print `[H] function:line` |
| `logger.setLevel(XLogLevel)` / `level()` | Runtime level: `None`, `Error`, `Warn`, `Info`, `Debug` |
| `logger.enabled(XLogLevel)` | `true` if that level prints right now — use it to skip expensive work |
| `logger.setTimestamp(bool)` | Start lines with `millis()` |
| `logger.setTag("IMU")` | Add `[IMU]` after the level; `nullptr` removes it |
| `logger.setOutput(Serial1)` / `output()` | Change where lines go |
| `setDebugMode(bool)`, `print(x)`, `println(x)` | Old API kept for compatibility (debug level, no prefix) |

## Examples

| Example | Shows |
|---|---|
| [`Basic`](examples/Basic/Basic.ino) | All levels, mixed values, `XLOG_HERE` |
| [`LevelsAndTags`](examples/LevelsAndTags/LevelsAndTags.ino) | Change the level over Serial, tags, timestamps, one logger per module |
| [`CompileTimeStrip`](examples/CompileTimeStrip/CompileTimeStrip.ino) | Release build without debug/info; methods vs macros |
| [`PlatformIO`](extras/platformio-example) | Full project with debug and release environments |

### Notes

- `uint64_t` / `int64_t` can't be printed on AVR (the core's `Print` doesn't support them).
- No format options: floats print with 2 decimals and there is no `HEX`.
- Not thread-safe: on ESP32 with several tasks, lines may interleave.

## Using with AI assistants

AI models don't know xLogger out of the box and tend to fall back to `Serial.print` and
`String`. This repository ships a reference written for them — rules, full API, patterns and
common mistakes — and CI compiles every code example in it.

| Option | Works with | How |
|---|---|---|
| **Attach a file** | Any chat AI | Download [`llms-full.txt`](https://raw.githubusercontent.com/YuriCirino/xlogger/main/llms-full.txt) and attach it to your request, or paste the URL if your AI can browse |
| **Agent Skill** | Claude, Copilot, Cursor, Codex, Gemini CLI and [others](https://agentskills.io/) | Copy [`skills/xlogger`](skills/xlogger) into your tool's skills folder (e.g. `.claude/skills/xlogger/` for Claude Code). The agent loads it when your project uses xLogger |
| **Already in your project** | Coding agents in PlatformIO projects | The file comes with the library: ask the agent to read `.pio/libdeps/<env>/xLogger/llms-full.txt` |

Example prompt:

> Using the xLogger library as described in the attached file, add logging to this ESP32
> firmware: debug logs for sensor readings, error logs for Wi-Fi failures, and a
> `platformio.ini` with debug and release environments.

The repository also has an [`llms.txt`](llms.txt) index and an [`AGENTS.md`](AGENTS.md)
for agents contributing to xLogger itself.

## Feedback, bugs and contributing

Suggestions and bug reports are very welcome, in English or Portuguese.

- 🐛 **Found a bug?** [Open a bug report](https://github.com/YuriCirino/xlogger/issues/new?template=bug_report.yml)
- 💡 **Have an idea?** [Suggest a feature](https://github.com/YuriCirino/xlogger/issues/new?template=feature_request.yml)
- 🔀 **Want to contribute code?** Read [CONTRIBUTING.md](CONTRIBUTING.md) and open a pull request.

Please search the [existing issues](https://github.com/YuriCirino/xlogger/issues) first,
and follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## License

[MIT](LICENSE) © Yuri Cirino

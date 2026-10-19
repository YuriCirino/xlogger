# xLogger

[![CI](https://github.com/YuriCirino/xlogger/actions/workflows/ci.yml/badge.svg)](https://github.com/YuriCirino/xlogger/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Lightweight leveled logger for Arduino and PlatformIO. Turn debug output on and off
without commenting lines, and strip it from release firmware entirely.

**English** · [Português](docs/GUIA-pt-BR.md)

> [!NOTE]
> 🇧🇷 **Fala português?** Existe uma página separada com o guia completo, passo a passo, em português:
> **[clique aqui para abrir o guia em português](docs/GUIA-pt-BR.md)**.

```cpp
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  XLOG_INFO(logger, F("boot ok"));
}

void loop() {
  int adc = analogRead(A0);
  XLOG_DEBUG(logger, F("adc="), adc, F(" volts="), adc * 5.0 / 1023);
  XLOG_HERE(logger);
}
```

```
[I] boot ok
[D] adc=512 volts=2.50
[H] loop:12
```

## Features

- **Four levels** — `error`, `warn`, `info`, `debug` — filtered at runtime with `setLevel()`.
- **Compile-time removal** — `-D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN` removes debug/info calls,
  their strings and their argument evaluation from the binary.
- **No `String`** — arguments are forwarded to `Print::print()` one by one: no heap
  allocation, no concatenation, `F()` strings stay in flash.
- **Any output** — takes a `Print&`: `Serial`, `Serial1`, USB CDC (Leonardo, ESP32-S3, RP2040),
  `SoftwareSerial`, an LCD, an SD `File`…
- **Caller location** — `XLOG_HERE(logger)` prints the calling function and line.
- **Timestamps and tags** — `[D][IMU] ...` with an optional `millis()` prefix; one logger per module.
- **Header-only**, no dependencies, C++11 (works with the AVR toolchain).

## Installation

### PlatformIO

In `platformio.ini`:

```ini
lib_deps =
    https://github.com/YuriCirino/xlogger.git#v1.0.0
```

### Arduino IDE

Download the repository as ZIP (*Code → Download ZIP*) and use
*Sketch → Include Library → Add .ZIP Library…*

## Usage

### Levels

| Macro | Method | Prefix | Level value |
|---|---|---|---|
| `XLOG_ERROR(lg, ...)` | `lg.error(...)` | `[E]` | `XLOG_LEVEL_ERROR` = 1 |
| `XLOG_WARN(lg, ...)`  | `lg.warn(...)`  | `[W]` | `XLOG_LEVEL_WARN` = 2 |
| `XLOG_INFO(lg, ...)`  | `lg.info(...)`  | `[I]` | `XLOG_LEVEL_INFO` = 3 |
| `XLOG_DEBUG(lg, ...)` | `lg.debug(...)` | `[D]` | `XLOG_LEVEL_DEBUG` = 4 |
| `XLOG_HERE(lg)`       | `lg.here(func, line)` | `[H]` | debug |

A message is printed when its level is ≤ the logger's level. `XLogLevel::None` silences everything.

**Prefer the macros.** The methods filter only at runtime; the macros also filter at compile time.

### Two filters

```ini
; platformio.ini — compile time: what goes INTO the firmware
build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN
```

```cpp
// runtime: what is PRINTED among what was compiled in
logger.setLevel(XLogLevel::Info);
```

> ⚠️ Disabled macros are removed together with their arguments. Never put side effects
> inside them: `XLOG_DEBUG(logger, i++)` will not increment `i` in a release build.

When using `#define` instead of build flags, it must come **before** `#include <xLogger.h>`
and only affects that file.

### API

| Member | Description |
|---|---|
| `XLogger(Print& out, XLogLevel level = XLogLevel::Debug)` | Constructor |
| `setLevel(XLogLevel)` / `level()` | Runtime level |
| `setOutput(Print&)` / `output()` | Change the output stream |
| `setTimestamp(bool)` | Prefix lines with `millis()` |
| `setTag(const char*)` | Add `[TAG]` after the level (`nullptr` removes it) |
| `enabled(XLogLevel)` | Whether that level would print now — useful to skip expensive work |
| `setDebugMode(bool)`, `print(x)`, `println(x)` | Compatibility API (debug level, no prefix) |

### Examples

| Example | Shows |
|---|---|
| [`Basic`](examples/Basic/Basic.ino) | All levels, mixed argument types, `XLOG_HERE` |
| [`LevelsAndTags`](examples/LevelsAndTags/LevelsAndTags.ino) | Runtime level change over Serial, tags, timestamps, one logger per module |
| [`CompileTimeStrip`](examples/CompileTimeStrip/CompileTimeStrip.ino) | Release build without debug/info |
| [`PlatformIO`](extras/platformio-example) | Full project with debug and release environments |

## Notes

- `uint64_t` / `int64_t` cannot be printed on AVR because the core's `Print` has no overload for them.
- Each `XLogger` costs a few bytes of RAM (pointer, level, flags, tag pointer).
- Messages are not thread-safe on multi-core targets: lines from different tasks may interleave.

## Using xLogger with AI assistants

AI models don't know xLogger out of the box and tend to fall back to `Serial.print` and
`String`. The repository ships a reference written for them — rules, full API, patterns and
anti-patterns — and CI compiles every code example in it.

| Option | Works with | How |
|---|---|---|
| **Attach a file** | Any chat AI | Download [`llms-full.txt`](https://raw.githubusercontent.com/YuriCirino/xlogger/main/llms-full.txt) and attach it to your request, or paste the URL if your AI can browse |
| **Agent Skill** | Claude, Copilot, Cursor, Codex, Gemini CLI and [others](https://agentskills.io/) | Copy [`skills/xlogger`](skills/xlogger) into your tool's skills folder (e.g. `.claude/skills/xlogger/` for Claude Code). The agent loads it automatically when your project uses xLogger |
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

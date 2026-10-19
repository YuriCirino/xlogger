// xLogger - lightweight leveled logger for Arduino / PlatformIO
// https://github.com/YuriCirino/xlogger
// SPDX-License-Identifier: MIT
#pragma once
#include <Arduino.h>

#define XLOGGER_VERSION "1.0.0"

// Level names for XLOGGER_MAX_LEVEL (usable in build flags).
#define XLOG_LEVEL_NONE  0
#define XLOG_LEVEL_ERROR 1
#define XLOG_LEVEL_WARN  2
#define XLOG_LEVEL_INFO  3
#define XLOG_LEVEL_DEBUG 4

// Highest level compiled into the firmware. XLOG_* macros above this level
// expand to nothing: no flash, no RAM, and their arguments are not evaluated.
// Set it in platformio.ini (build_flags = -D XLOGGER_MAX_LEVEL=2) or
// with #define BEFORE #include <xLogger.h>.
#ifndef XLOGGER_MAX_LEVEL
#define XLOGGER_MAX_LEVEL XLOG_LEVEL_DEBUG
#endif

enum class XLogLevel : uint8_t {
  None = XLOG_LEVEL_NONE,
  Error = XLOG_LEVEL_ERROR,
  Warn = XLOG_LEVEL_WARN,
  Info = XLOG_LEVEL_INFO,
  Debug = XLOG_LEVEL_DEBUG
};

class XLogger {
public:
  // Any Print works: HardwareSerial, USB CDC Serial (Leonardo, ESP32-S3,
  // RP2040), SoftwareSerial, an LCD, an SD File...
  explicit XLogger(Print &out, XLogLevel level = XLogLevel::Debug)
      : out_(&out), level_(level) {}

  // ---- configuration ----------------------------------------------------
  void setOutput(Print &out) { out_ = &out; }
  Print &output() const { return *out_; }

  void setLevel(XLogLevel level) { level_ = level; }
  XLogLevel level() const { return level_; }

  void setTimestamp(bool on) { timestamp_ = on; }  // prefix with millis()
  void setTag(const char *tag) { tag_ = tag; }      // e.g. "IMU" -> [D][IMU]

  bool enabled(XLogLevel l) const {
    return l != XLogLevel::None &&
           static_cast<uint8_t>(l) <= static_cast<uint8_t>(level_);
  }

  // ---- logging ----------------------------------------------------------
  // Each call prints "[X] arg1arg2...\n". Arguments go straight to
  // Print::print(), so no String concatenation and F() strings stay in flash:
  //   log.debug(F("adc="), adc, F(" v="), v);
  template <typename... Args> void error(const Args &...args) { log(XLogLevel::Error, 'E', args...); }
  template <typename... Args> void warn(const Args &...args)  { log(XLogLevel::Warn,  'W', args...); }
  template <typename... Args> void info(const Args &...args)  { log(XLogLevel::Info,  'I', args...); }
  template <typename... Args> void debug(const Args &...args) { log(XLogLevel::Debug, 'D', args...); }

  // Prints the caller's function and line. Use it through XLOG_HERE(logger),
  // which captures __func__ and __LINE__ at the call site.
  void here(const char *func, int line) {
    if (!enabled(XLogLevel::Debug)) return;
    prefix('H');
    out_->print(func);
    out_->print(':');
    out_->println(line);
  }

  // ---- backward compatibility with the original Logger API --------------
  void setDebugMode(bool on) { level_ = on ? XLogLevel::Debug : XLogLevel::None; }
  template <typename T> void print(const T &v)   { if (enabled(XLogLevel::Debug)) out_->print(v); }
  template <typename T> void println(const T &v) { if (enabled(XLogLevel::Debug)) out_->println(v); }
  void println() { if (enabled(XLogLevel::Debug)) out_->println(); }

private:
  Print *out_;  // pointer, not reference, so setOutput() can re-point it
  XLogLevel level_;
  bool timestamp_ = false;
  const char *tag_ = nullptr;

  void prefix(char lvl) {
    if (timestamp_) {
      out_->print(millis());
      out_->print(' ');
    }
    out_->print('[');
    out_->print(lvl);
    out_->print(']');
    if (tag_) {
      out_->print('[');
      out_->print(tag_);
      out_->print(']');
    }
    out_->print(' ');
  }

  template <typename... Args>
  void log(XLogLevel l, char lvl, const Args &...args) {
    if (!enabled(l)) return;
    prefix(lvl);
    printAll(args...);
    out_->println();
  }

  void printAll() {}
  template <typename T, typename... Rest>
  void printAll(const T &first, const Rest &...rest) {
    out_->print(first);
    printAll(rest...);
  }
};

// ---- compile-time filtered macros ----------------------------------------
// Disabled levels vanish from the binary, so never put side effects
// (i++, function calls that change state) in their arguments.
#if XLOGGER_MAX_LEVEL >= XLOG_LEVEL_ERROR
#define XLOG_ERROR(lg, ...) (lg).error(__VA_ARGS__)
#else
#define XLOG_ERROR(lg, ...) ((void)0)
#endif

#if XLOGGER_MAX_LEVEL >= XLOG_LEVEL_WARN
#define XLOG_WARN(lg, ...) (lg).warn(__VA_ARGS__)
#else
#define XLOG_WARN(lg, ...) ((void)0)
#endif

#if XLOGGER_MAX_LEVEL >= XLOG_LEVEL_INFO
#define XLOG_INFO(lg, ...) (lg).info(__VA_ARGS__)
#else
#define XLOG_INFO(lg, ...) ((void)0)
#endif

#if XLOGGER_MAX_LEVEL >= XLOG_LEVEL_DEBUG
#define XLOG_DEBUG(lg, ...) (lg).debug(__VA_ARGS__)
#define XLOG_HERE(lg) (lg).here(__func__, __LINE__)
#else
#define XLOG_DEBUG(lg, ...) ((void)0)
#define XLOG_HERE(lg) ((void)0)
#endif

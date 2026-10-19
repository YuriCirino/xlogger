// Minimal Arduino mock so xLogger can be unit-tested on the host (g++).
#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

class __FlashStringHelper;
#define F(s) (reinterpret_cast<const __FlashStringHelper *>(s))

inline unsigned long &mockMillis() { static unsigned long t = 0; return t; }
inline unsigned long millis() { return mockMillis(); }

// Captures everything printed into a std::string.
class Print {
public:
  std::string buf;
  void print(const char *s) { buf += s; }
  void print(const __FlashStringHelper *s) { buf += reinterpret_cast<const char *>(s); }
  void print(char c) { buf += c; }
  void print(int v) { buf += std::to_string(v); }
  void print(unsigned int v) { buf += std::to_string(v); }
  void print(long v) { buf += std::to_string(v); }
  void print(unsigned long v) { buf += std::to_string(v); }
  void print(double v) { char t[32]; snprintf(t, sizeof t, "%.2f", v); buf += t; }
  void println() { buf += "\r\n"; }
  template <typename T> void println(const T &v) { print(v); println(); }
};

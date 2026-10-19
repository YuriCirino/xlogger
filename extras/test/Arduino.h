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

// ---- extra stubs so documentation snippets can be syntax-checked ----------
#include <cmath>
using std::sqrt;

class MockSerial : public Print {
public:
  void begin(unsigned long) {}
  int available() { return 0; }
  int read() { return -1; }
  explicit operator bool() const { return true; }
};
extern MockSerial Serial, Serial1;

enum { A0 = 14, INPUT = 0, OUTPUT = 1, INPUT_PULLUP = 2, FALLING = 2, RISING = 3, CHANGE = 1 };
inline int analogRead(int) { return 0; }
inline void delay(unsigned long) {}
inline void pinMode(int, int) {}
inline int digitalPinToInterrupt(int p) { return p; }
inline void attachInterrupt(int, void (*)(), int) {}

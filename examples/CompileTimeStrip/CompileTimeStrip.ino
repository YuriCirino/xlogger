// Release build: strip debug and info logs from the firmware.
//
// XLOGGER_MAX_LEVEL must be defined BEFORE the include. In PlatformIO prefer
// build_flags in platformio.ini (it applies to every file):
//   build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN
//
// Compile with and without the #define below and compare the flash usage.
#define XLOGGER_MAX_LEVEL XLOG_LEVEL_WARN
#include <xLogger.h>

XLogger logger(Serial);

int expensiveDiagnostics() {
  // Never called in this build: disabled XLOG_* arguments are not evaluated.
  long sum = 0;
  for (int i = 0; i < 1000; i++) sum += analogRead(A0);
  return sum / 1000;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

void loop() {
  XLOG_DEBUG(logger, F("avg adc = "), expensiveDiagnostics());  // removed
  XLOG_INFO(logger, F("this string is not even in flash"));     // removed
  XLOG_WARN(logger, F("warnings stay"));                        // kept
  XLOG_ERROR(logger, F("errors stay"));                         // kept
  delay(1000);
}

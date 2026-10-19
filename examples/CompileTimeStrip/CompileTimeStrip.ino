// Release build: remove debug and info logs from the firmware.
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
  long sum = 0;
  for (int i = 0; i < 1000; i++) sum += analogRead(A0);
  return sum / 1000;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}
}

void loop() {
  // Methods: removed from the firmware, but the arguments still run, so
  // expensiveDiagnostics() is called here even in this release build.
  logger.debug("avg adc = ", expensiveDiagnostics());
  logger.info("this text is not in the firmware");

  // Macros: the whole line disappears, arguments included, so
  // expensiveDiagnostics() is NOT called here. Never put side effects inside.
  XLOG_DEBUG(logger, "avg adc = ", expensiveDiagnostics());

  logger.warn("warnings stay");  // kept
  logger.error("errors stay");   // kept
  delay(1000);
}

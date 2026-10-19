// Used by CI to prove that disabled levels are removed from the firmware.
// Built twice (XLOGGER_MAX_LEVEL=4 and =2); CI then searches the ELF for the
// marker strings. Not an example: see examples/ for usage.
#include <xLogger.h>

XLogger logger(Serial);

void setup() { Serial.begin(115200); }

void loop() {
  int v = analogRead(A0);
  logger.debug(F("STRIP_MARKER_METHOD_DEBUG "), v);
  logger.info(F("STRIP_MARKER_METHOD_INFO"));
  XLOG_DEBUG(logger, F("STRIP_MARKER_MACRO_DEBUG "), v);
  XLOG_INFO(logger, F("STRIP_MARKER_MACRO_INFO"));
  logger.warn(F("KEEP_MARKER_METHOD_WARN "), v);
  XLOG_ERROR(logger, F("KEEP_MARKER_MACRO_ERROR"));
  delay(1000);
}

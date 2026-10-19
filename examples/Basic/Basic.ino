// Basic usage: one logger on Serial, all levels enabled.
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}  // USB boards: wait up to 3 s for the PC

  XLOG_INFO(logger, F("xLogger "), XLOGGER_VERSION, F(" ready"));
}

void loop() {
  int adc = analogRead(A0);
  float volts = adc * 5.0f / 1023.0f;

  // Any mix of types; no String() or "+" needed.
  XLOG_DEBUG(logger, F("adc="), adc, F(" volts="), volts);

  if (volts > 4.5f) XLOG_WARN(logger, F("high voltage: "), volts);
  if (adc == 0)     XLOG_ERROR(logger, F("sensor disconnected?"));

  XLOG_HERE(logger);  // prints "[H] loop:<line number>"

  delay(1000);
}

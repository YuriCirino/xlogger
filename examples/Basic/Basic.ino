// Basic usage: one logger on Serial, all levels enabled.
// If you know Serial.print(), you already know xLogger: separate values with commas.
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}  // USB boards: wait up to 3 s for the PC

  logger.info("xLogger ", XLOGGER_VERSION, " ready");
}

void loop() {
  int adc = analogRead(A0);
  float volts = adc * 5.0f / 1023.0f;

  // Any mix of types; no String() or "+" needed.
  logger.debug("adc=", adc, " volts=", volts);

  if (volts > 4.5f) logger.warn("high voltage: ", volts);
  if (adc == 0)     logger.error("sensor disconnected?");

  // F("...") keeps the text in flash instead of RAM (important on the Uno).
  logger.debug(F("this text lives in flash"));

  XLOG_HERE(logger);  // prints "[H] loop:<line number>"

  delay(1000);
}

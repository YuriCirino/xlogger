#include <Arduino.h>
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  logger.setTimestamp(true);
  logger.info(F("boot - xLogger "), XLOGGER_VERSION);
}

void loop() {
  static uint32_t counter = 0;
  counter++;

  logger.debug(F("counter="), counter);       // gone in uno_release
  if (counter % 10 == 0) logger.warn(F("10 more iterations"));

  delay(500);
}

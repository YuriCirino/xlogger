// Runtime level changes, timestamps and one logger per module (tags).
// Send 'd', 'i', 'w', 'e' or 'n' over the Serial Monitor to change the level.
#include <xLogger.h>

XLogger sensorLog(Serial);
XLogger motorLog(Serial);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}

  sensorLog.setTag("SENSOR");
  motorLog.setTag("MOTOR");
  sensorLog.setTimestamp(true);
  motorLog.setTimestamp(true);

  motorLog.setLevel(XLogLevel::Warn);  // motor module: only warnings and errors
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    XLogLevel l = sensorLog.level();
    switch (c) {
      case 'd': l = XLogLevel::Debug; break;
      case 'i': l = XLogLevel::Info;  break;
      case 'w': l = XLogLevel::Warn;  break;
      case 'e': l = XLogLevel::Error; break;
      case 'n': l = XLogLevel::None;  break;
    }
    sensorLog.setLevel(l);
    Serial.print(F("sensor level = "));
    Serial.println(static_cast<int>(l));
  }

  static uint16_t n = 0;
  n++;

  XLOG_DEBUG(sensorLog, F("sample #"), n);
  XLOG_INFO(sensorLog, F("temperature "), 25.3f, F(" C"));
  XLOG_INFO(motorLog, F("rpm "), 1200);           // filtered (level = Warn)
  if (n % 5 == 0) XLOG_WARN(motorLog, F("current peak"));

  // Output example:
  //   10234 [D][SENSOR] sample #10
  //   10234 [I][SENSOR] temperature 25.30 C
  //   10235 [W][MOTOR] current peak

  delay(1000);
}

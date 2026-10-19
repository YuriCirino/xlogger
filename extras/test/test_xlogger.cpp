// Host unit tests for xLogger with every level compiled in (default).
// Run all host tests from the repository root with: extras/test/run.sh
#include "xLogger.h"
#include "test_common.h"

static void testFormattingAndTypes() {
  Print p;
  XLogger log(p);
  log.info(F("adc="), 512, F(" v="), 2.5f, ' ', "ok", 7UL);
  CHECK_EQ(p.buf, "[I] adc=512 v=2.50 ok7\r\n");
}

static void testRuntimeLevels() {
  Print p;
  XLogger log(p, XLogLevel::Warn);
  log.debug("d");
  log.info("i");
  log.warn("w");
  log.error("e");
  CHECK_EQ(p.buf, "[W] w\r\n[E] e\r\n");

  p.buf.clear();
  log.setLevel(XLogLevel::None);
  log.error("silenced");
  CHECK_EQ(p.buf, "");
}

static void testMethodsAndMacrosPrintTheSame() {
  Print a, b;
  XLogger viaMethod(a), viaMacro(b);
  viaMethod.debug(F("x="), 1);
  XLOG_DEBUG(viaMacro, F("x="), 1);
  CHECK_EQ(a.buf, "[D] x=1\r\n");
  CHECK_EQ(b.buf, a.buf);
}

static void testTimestampAndTag() {
  Print p;
  XLogger log(p);
  mockMillis() = 1234;
  log.setTimestamp(true);
  log.setTag("IMU");
  log.warn("tilt");
  CHECK_EQ(p.buf, "1234 [W][IMU] tilt\r\n");
}

static void testHere() {
  Print p;
  XLogger log(p);
  log.here("loop", 42);
  CHECK_EQ(p.buf, "[H] loop:42\r\n");
}

static void testSetOutput() {
  Print a, b;
  XLogger log(a);
  log.info("to a");
  log.setOutput(b);
  log.info("to b");
  CHECK_EQ(a.buf, "[I] to a\r\n");
  CHECK_EQ(b.buf, "[I] to b\r\n");
}

static void testEnabled() {
  Print p;
  XLogger log(p, XLogLevel::Info);
  CHECK(log.enabled(XLogLevel::Error), "Error should be enabled at Info");
  CHECK(log.enabled(XLogLevel::Info), "Info should be enabled at Info");
  CHECK(!log.enabled(XLogLevel::Debug), "Debug should be off at Info");
  CHECK(!log.enabled(XLogLevel::None), "None is never 'enabled'");
}

static void testCompatApi() {
  Print p;
  XLogger log(p, XLogLevel::None);
  log.println("hidden");
  log.setDebugMode(true);
  log.print("x=");
  log.println(3);
  CHECK_EQ(p.buf, "x=3\r\n");
}

int main() {
  testFormattingAndTypes();
  testRuntimeLevels();
  testMethodsAndMacrosPrintTheSame();
  testTimestampAndTag();
  testHere();
  testSetOutput();
  testEnabled();
  testCompatApi();
  return finish("all levels compiled in");
}

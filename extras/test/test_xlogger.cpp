// Host unit tests for xLogger.
// From the repository root:
//   g++ -std=gnu++11 -Wall -Wextra -Werror -I extras/test -I src
//       extras/test/test_xlogger.cpp -o extras/test/test_xlogger
//   ./extras/test/test_xlogger
#define XLOGGER_MAX_LEVEL XLOG_LEVEL_INFO  // DEBUG is compiled out in this file
#include "xLogger.h"

#include <cstdlib>
#include <cstring>

static int failures = 0;

#define CHECK_EQ(actual, expected)                                            \
  do {                                                                        \
    std::string a_ = (actual), e_ = (expected);                               \
    if (a_ != e_) {                                                           \
      failures++;                                                             \
      printf("FAIL %s:%d\n  expected: \"%s\"\n  actual:   \"%s\"\n", __FILE__, \
             __LINE__, e_.c_str(), a_.c_str());                               \
    }                                                                         \
  } while (0)

static int sideEffects = 0;
// Only referenced inside a compiled-out macro, hence "unused".
__attribute__((unused)) static int touch() { return ++sideEffects; }

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

static void testCompileTimeStrip() {
  Print p;
  XLogger log(p, XLogLevel::Debug);
  XLOG_DEBUG(log, "gone ", touch());  // compiled out: touch() never runs
  XLOG_HERE(log);                     // compiled out too
  XLOG_INFO(log, "kept");
  CHECK_EQ(p.buf, "[I] kept\r\n");
  if (sideEffects != 0) {
    failures++;
    printf("FAIL: disabled macro evaluated its arguments\n");
  }
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
  testCompileTimeStrip();
  testTimestampAndTag();
  testHere();
  testSetOutput();
  testCompatApi();

  if (failures) {
    printf("%d check(s) failed\n", failures);
    return EXIT_FAILURE;
  }
  printf("All tests passed\n");
  return EXIT_SUCCESS;
}

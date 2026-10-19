// Host unit tests for compile-time removal: DEBUG is compiled out here.
// Run all host tests from the repository root with: extras/test/run.sh
#define XLOGGER_MAX_LEVEL XLOG_LEVEL_INFO
#include "xLogger.h"
#include "test_common.h"

static void testMacrosSkipArguments() {
  Print p;
  XLogger log(p, XLogLevel::Debug);  // runtime allows Debug, compile time does not
  sideEffects = 0;
  XLOG_DEBUG(log, "gone ", touch());
  XLOG_HERE(log);
  XLOG_INFO(log, "kept");
  CHECK_EQ(p.buf, "[I] kept\r\n");
  CHECK(sideEffects == 0, "a compiled-out macro must not evaluate its arguments");
}

static void testMethodsStillEvaluateArguments() {
  Print p;
  XLogger log(p, XLogLevel::Debug);
  sideEffects = 0;
  log.debug("gone ", touch());
  log.info("kept");
  CHECK_EQ(p.buf, "[I] kept\r\n");
  CHECK(sideEffects == 1, "a compiled-out method must still evaluate its arguments once");
}

static void testEverythingDebugLevelIsOff() {
  Print p;
  XLogger log(p, XLogLevel::Debug);
  log.here("loop", 1);
  log.print("compat ");
  log.println("api");
  log.println();
  CHECK_EQ(p.buf, "");
  CHECK(!log.enabled(XLogLevel::Debug), "enabled(Debug) must be false when compiled out");
  CHECK(log.enabled(XLogLevel::Info), "enabled(Info) must stay true");
}

int main() {
  testMacrosSkipArguments();
  testMethodsStillEvaluateArguments();
  testEverythingDebugLevelIsOff();
  return finish("XLOGGER_MAX_LEVEL=INFO");
}

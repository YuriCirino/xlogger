// Tiny test helpers shared by the host test programs.
#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>

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

#define CHECK(cond, msg)                                         \
  do {                                                           \
    if (!(cond)) {                                               \
      failures++;                                                \
      printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, msg);       \
    }                                                            \
  } while (0)

// Counts how many times a log argument was evaluated.
static int sideEffects = 0;
__attribute__((unused)) static int touch() { return ++sideEffects; }

static int finish(const char *suite) {
  if (failures) {
    printf("%s: %d check(s) failed\n", suite, failures);
    return EXIT_FAILURE;
  }
  printf("%s: all tests passed\n", suite);
  return EXIT_SUCCESS;
}

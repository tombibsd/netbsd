/*	$NetBSD$	*/

/* AUTOGENERATED FILE. DO NOT EDIT. */

//=======Test Runner Used To Run Each Test Below=====
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT() && !TEST_IS_IGNORED) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

//=======Automagically Detected Files To Include=====
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include "config.h"
#include "ntp_calendar.h"

//=======External Functions This Runner Calls=====
extern void setUp(void);
extern void tearDown(void);
extern void test_DateGivenMonthDay(void);
extern void test_DateGivenYearDay(void);
extern void test_DateLeapYear(void);
extern void test_WraparoundDateIn2036(void);


//=======Test Reset Option=====
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}

char const *progname;


//=======MAIN=====
int main(int argc, char *argv[])
{
  progname = argv[0];
  UnityBegin("caltontp.c");
  RUN_TEST(test_DateGivenMonthDay, 5);
  RUN_TEST(test_DateGivenYearDay, 6);
  RUN_TEST(test_DateLeapYear, 7);
  RUN_TEST(test_WraparoundDateIn2036, 8);

  return (UnityEnd());
}

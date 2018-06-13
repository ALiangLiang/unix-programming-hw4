#include <stdio.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>

#include "hw4.h"

using namespace std;
using ::testing::ContainsRegex;
using ::testing::MatchesRegex;

namespace {
  TEST(Hw4SpecTest, printIfs) {
    string ifs = printIfs();
    char regexTpl[512];
    strcat(regexTpl, "^.{1,20} {,20}"); // interface name
    strcat(regexTpl, "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} "); // IPv4
    strcat(regexTpl, "0x[0-9a-f]{8} "); // netmask
    strcat(regexTpl, "\\([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\) "); // broadcast address
    strcat(regexTpl, "[0-9][0-9]:[0-9][0-9]:[0-9][0-9]:[0-9][0-9]:[0-9][0-9]:[0-9][0-9]"); // MAC
    ASSERT_THAT(ifs, ContainsRegex(regexTpl));
  }

  TEST(Hw4SpecTest, communication) {
    // char name[32] = "foo";
    // ASSERT_GT(communication(name), "foo");
  }
}

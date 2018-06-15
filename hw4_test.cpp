#include <stdio.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <iostream>

#include "hw4.h"

#define IF_NAME "eth0"
#define MAC "8A:CA:58:B9:E9:51"

using namespace std;
using ::testing::ContainsRegex;
using ::testing::MatchesRegex;

namespace {
  TEST(Hw4SpecTest, getMac) {
    struct If if_info;
    char if_name[] = IF_NAME;
    getMac(if_info.mac, if_info.hwaddr, if_name);
    ASSERT_STREQ(MAC, if_info.mac);
  }

  TEST(Hw4SpecTest, sprintIfs) {
    char buf[128];
    struct If if_info;
    if_info.index = 1;
    strncpy(if_info.name, "foo", 20);
    strncpy(if_info.ip, "192.168.94.87", 16);
    strncpy(if_info.netmask, "255.255.255.0", 16);
    strncpy(if_info.broadcast, "192.168.94.255", 16);
    strncpy(if_info.mac, "aa:bb:cc:dd:ee:ff", 19);
    sprintIf(buf, &if_info);

    char regexTpl[512] = "";
    //memset(regexTpl, "\0", 1);
    strcat(regexTpl, "^ *[0-9]+ - .{1,20} {,20}"); // interface name
    strcat(regexTpl, "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} *"); // IPv4
    strcat(regexTpl, "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} *"); // netmask
    strcat(regexTpl, "\\( *[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\) "); // broadcast address
    strcat(regexTpl, "[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]"); // MAC
    ASSERT_THAT(buf, ContainsRegex(regexTpl));
  }

  TEST(Hw4SpecTest, getIfs) {
    struct If ifs[32];
    int if_count = getIfs(ifs);
    ASSERT_EQ(1, if_count);
    ASSERT_STREQ("eth0", ifs[0].name);
  }

  TEST(Hw4SpecTest, sendMsg) {
    char name[32] = "foo";
    char msg[256] = "bar";
    struct If ifs[32];
    int if_count = getIfs(ifs);
    sendMsg(name, msg, ifs, if_count);
  }
}

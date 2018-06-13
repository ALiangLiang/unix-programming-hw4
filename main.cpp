#define SOCK_PATH "/dev/socket/echo_socket"

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "hw4.h"


using namespace std;

int main (int argc, char* argv[]) {
  char name[32];
  pid_t pid = fork();

  if (pid > 0) {
    printf("Enumerated network interfaces:\n");
    struct If ifs[32];
    int if_count = getIfs(ifs);
    for (int i = 0; i < if_count; i++) {
      // cout << ifs[i].mac << endl;
      char if_info[128];
      sprintIf(if_info, &ifs[i]);
      cout << if_info << endl;
    }
  
    printf("Enter your name: ");
    scanf("%32s", name);
    printf("Welcome, '%s'\n", name);
    cout << if_count << endl;

    while (true) {
      char msg[256];
      scanf("%s", msg);
      sendMsg(name, msg, ifs, if_count);
    }
  } else if (pid == 0) {
    listen();
  }
}

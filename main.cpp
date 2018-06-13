#define SOCK_PATH "/dev/socket/echo_socket"

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "hw4.h"

using namespace std;

int main (int argc, char* argv[]) {
  char name[32];
  pid_t pid = fork();

  struct If ifs[32];
  int if_count = getIfs(ifs);

  if (pid > 0) {
    printf("Enumerated network interfaces:\n");
    for (int i = 0; i < if_count; i++) {
      char if_info[128];
      sprintIf(if_info, &ifs[i]);
      cout << if_info << endl;
    }
  
    printf("Enter your name: ");
    if (scanf("%31s", name) < 0)
      perror("scanf name");
    printf("Welcome, '%s'\n", name);

    while (true) {
      char msg[256];
      if (scanf("%s", msg) < 0)
        perror("scanf msg");
      sendMsg(name, msg, ifs, if_count);
    }
  } else if (pid == 0) {
    listen(ifs);
  }
}

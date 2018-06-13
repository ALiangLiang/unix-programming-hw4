#define SOCK_PATH "/dev/socket/echo_socket"

#include <stdio.h>
#include <iostream>

#include "hw4.h"


using namespace std;

int main (int argc, char* argv[]) {
  char name[32];

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
  communication(name, ifs, if_count);
}

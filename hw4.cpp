#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <iostream>
#include <errno.h>

#include "hw4.h"

#define ETH_PROTO ETH_P_IP + 1

using namespace std;

void getMac (char* mac, char* dv) {
    struct ifreq ifr;
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      return;
    }
    strcpy(ifr.ifr_name, dv);
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
      perror("ioctl");
      return;
    }
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
           (unsigned char)ifr.ifr_hwaddr.sa_data[0],
           (unsigned char)ifr.ifr_hwaddr.sa_data[1],
           (unsigned char)ifr.ifr_hwaddr.sa_data[2],
           (unsigned char)ifr.ifr_hwaddr.sa_data[3],
           (unsigned char)ifr.ifr_hwaddr.sa_data[4],
           (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
}

void sprintIf (char *buf, If *if_info) {
  sprintf(buf, "%2d - %-20s %15s %15s (%15s) %17s",
        if_info->index,
        if_info->name,
        if_info->ip,
        if_info->netmask,
        if_info->broadcast,
        if_info->mac);
}

int getIfs (If *ifs) {
  struct ifaddrs * ifAddrStruct = NULL;
  void * tmpAddrPtr = NULL;
  string ifInfo = "";
  int if_count = 0;
  struct ifreq ifr;
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if(sock < 0){
    printf("socket: %s", strerror(errno));
    return 0;
  }

  getifaddrs(&ifAddrStruct);

  while (ifAddrStruct != NULL) {
		if (strcmp(ifAddrStruct->ifa_name, "lo") != 0) {
      if (ifAddrStruct->ifa_addr->sa_family == AF_INET && if_count < 32) {
        // Get name
        ifs[if_count].name = ifAddrStruct->ifa_name;
        // Get IP
        inet_ntop(AF_INET, &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr, ifs[if_count].ip, INET_ADDRSTRLEN);
        // Get netmask
        inet_ntop(AF_INET, &(((struct sockaddr_in *)ifAddrStruct->ifa_netmask)->sin_addr), ifs[if_count].netmask, INET_ADDRSTRLEN);
        // Get broadcast
        inet_ntop(AF_INET, &(((struct sockaddr_in *)ifAddrStruct->ifa_broadaddr)->sin_addr), ifs[if_count].broadcast, INET_ADDRSTRLEN);
        // Get MAC
        getMac(ifs[if_count].mac, ifAddrStruct->ifa_name);
        // Get index
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, ifAddrStruct->ifa_name);
        if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
          printf("errpr: %s", strerror(errno));
        ifs[if_count].index = ifr.ifr_ifindex;
        if_count++;
      }
  	}
    ifAddrStruct = ifAddrStruct->ifa_next;
  }

  return if_count;
}

void communication (char* name, If *ifs, int if_count) {
  // Open raw socket.
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  struct ethhdr *eth;
	char buf[BUFSIZ];
  fd_set fds;
  size_t size;

  while (true) {
    char msg[256];

    scanf("%s", msg);

    FD_ZERO(&fds); // Initialize fdset
		FD_SET(sock, &fds); // Sets the bit for the file descriptor fd in the file descriptor set fdset.

		sprintf(buf + sizeof(struct ethhdr), "[%s]: %s", name, msg);
		eth = (struct ethhdr *)(buf);
    for (int i = 0; i < if_count; i++) {
      char *ifsaddr;
      inet_pton(AF_INET, ifs[i].ip, ifsaddr);
      eth->h_source[0] = (unsigned char)(ifsaddr[0]);
      eth->h_source[1] = (unsigned char)(ifsaddr[1]);
      eth->h_source[2] = (unsigned char)(ifsaddr[2]);
      eth->h_source[3] = (unsigned char)(ifsaddr[3]);
      eth->h_source[4] = (unsigned char)(ifsaddr[4]);
      eth->h_source[5] = (unsigned char)(ifsaddr[5]);
      eth->h_dest[0] = eth->h_dest[1] = eth->h_dest[2] = 0xff;
      eth->h_dest[3] = eth->h_dest[4] = eth->h_dest[5] = 0xff;
      eth->h_proto = htons(ETH_PROTO);

      struct sockaddr_ll sadr_ll;
      sadr_ll.sll_ifindex = ifs[i].index;
      sadr_ll.sll_halen = ETH_ALEN;
      sadr_ll.sll_addr[0] = sadr_ll.sll_addr[1] = sadr_ll.sll_addr[2] = 0xff;
      sadr_ll.sll_addr[3] = sadr_ll.sll_addr[4] = sadr_ll.sll_addr[5] = 0xff;
      size = sizeof(struct ethhdr) + strlen(buf + sizeof(struct ethhdr));
  		// Send packet
  		sendto(sock, buf, size, 0, (const struct sockaddr*)&sadr_ll, sizeof(struct sockaddr_ll));
    }
  }
}

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

void getMac (char* mac, char* hwaddr, char* dv) {
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
    memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, 6);
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
        strncpy(ifs[if_count].name, ifAddrStruct->ifa_name, 20);
        // Get IP
        inet_ntop(AF_INET, &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr, ifs[if_count].ip, INET_ADDRSTRLEN);
        // Get netmask
        inet_ntop(AF_INET, &(((struct sockaddr_in *)ifAddrStruct->ifa_netmask)->sin_addr), ifs[if_count].netmask, INET_ADDRSTRLEN);
        // Get broadcast
        inet_ntop(AF_INET, &(((struct sockaddr_in *)ifAddrStruct->ifa_broadaddr)->sin_addr), ifs[if_count].broadcast, INET_ADDRSTRLEN);
        // Get MAC
        getMac(ifs[if_count].mac, ifs[if_count].hwaddr, ifAddrStruct->ifa_name);
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

void sendMsg (char* name, char* msg, If *ifs, int if_count) {
  // Open raw socket.
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  struct ethhdr *eth;
  char buf[BUFSIZ];
  fd_set fds;
  size_t size;
    
  FD_ZERO(&fds); // Initialize fdset
  FD_SET(sock, &fds); // Sets the bit for the file descriptor fd in the file descriptor set fdset.

  sprintf(buf + sizeof(struct ethhdr), "[%s]: %s", name, msg);
  eth = (struct ethhdr *)(buf);
  for (int i = 0; i < if_count; i++) {
    char ifsaddr[616];
    inet_pton(AF_INET, ifs[i].mac, ifsaddr);

    for (int j = 0; j < 6; j++) {
      eth->h_source[j] = (unsigned char)(ifs[i].hwaddr[j]);
      eth->h_dest[j] = 0xff;
    }
    eth->h_proto = htons(ETH_PROTO);

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifs[i].index;
    sadr_ll.sll_halen = ETH_ALEN;
    for (int j = 0; j < 6; j++)
      sadr_ll.sll_addr[j] = 0xff;
    size = sizeof(struct ethhdr) + strlen(buf + sizeof(struct ethhdr));
    // Send packet
    sendto(sock, buf, size, 0, (const struct sockaddr*)&sadr_ll, sizeof(struct sockaddr_ll));
  }
}

bool isMACsSame (If *ifs, char *mac) {
  for (int i = 0; i < sizeof(ifs); i++)
    if (strcmp(ifs[i].mac, mac) == 0)
      return true;
  return false;
}

void listen (If *ifs) {
    //sleep(1);
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  while (true) {
    char buf[BUFSIZ];
    struct ethhdr *eth;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);
    ssize_t ssize = recvfrom(sock, buf, sizeof(buf), 0, &saddr, (socklen_t *)&saddr_len);
    if (ssize < 0) {
      printf("recvfrom error");
      return;
    }
    buf[ssize] = '\0';
    eth = (struct ethhdr *)(buf);
    if (ntohs(eth->h_proto) == ETH_PROTO){
      char mac[19];
      sprintf(mac, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
              eth->h_source[0], eth->h_source[1], eth->h_source[2],
              eth->h_source[3], eth->h_source[4], eth->h_source[5]);
      if (isMACsSame(ifs, mac))
        continue;
      printf(">>> <%s> %s\n", mac, buf + sizeof(struct ethhdr));
    } 
  }
}


#ifndef PING_H
#define PING_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int is_valid_ipv4(const char *ip);
unsigned short checksum(void *b, int len);

#endif
#include "ping.h"

int is_valid_ipv4(const char *ip) {
  struct sockaddr_in sa;

  return inet_pton(AF_INET, ip, &(sa.sin_addr.s_addr)) == 1;
}

unsigned short checksum(void *b, int len) {
  unsigned short *buf = b;
  unsigned int sum = 0;

  for (; len > 1; len -= 2) sum += *buf++;

  if (len == 1) sum += *((unsigned char *)buf) << 8;

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  return ~sum;
}
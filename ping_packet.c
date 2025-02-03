#include "ping.h"

void create_icmp_packet(char *packet, int seq,
                        int payload_size) {
  struct icmphdr *icmp = (struct icmphdr *)packet;
  struct timeval *time_sent =
      (struct timeval *)(packet + sizeof(struct icmphdr));

  // icmp->un.echo.id에 getpid() & 0xFFFF형태의 값을 넣는
  // 이유는 getpid() 값이 16비트 넘길 수 있기 때문.
  icmp->type = (uint8_t)ICMP_ECHO;
  icmp->un.echo.id = htons(getpid() & 0xFFFF);
  icmp->un.echo.sequence = htons(seq);
  icmp->code = 0;

  gettimeofday(time_sent, NULL);

  icmp->checksum = 0;
  icmp->checksum = checksum(packet, payload_size);
}

int receive_ping_reply(int sock, pid_t id, int seq,
                       struct timeval *rtt, int *ttl,
                       struct sockaddr_in *src_addr) {
  char buffer[1024];
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  ssize_t bytes;

  int ip_header_len;
  struct iphdr *ip;
  struct icmphdr *icmp_reply;
  struct timeval time_received, time_sent;

  do {
    bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                     (struct sockaddr *)&addr, &addr_len);
    gettimeofday(&time_received, NULL);
    if (bytes < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0;
      else {
        perror("recvfrom");
        exit(EXIT_FAILURE);
      }
    }

    ip = (struct iphdr *)buffer;
    ip_header_len = ip->ihl * 4;
    icmp_reply = (struct icmphdr *)(buffer + ip_header_len);
  } while (icmp_reply->type == ICMP_ECHO);

  *src_addr = addr;
  *ttl = ip->ttl;

  if (icmp_reply->type == ICMP_ECHOREPLY &&
      ntohs(icmp_reply->un.echo.id) == (id & 0xFFFF) &&
      ntohs(icmp_reply->un.echo.sequence) == seq) {
    memcpy(&time_sent,
           buffer + ip_header_len + sizeof(struct icmphdr),
           sizeof(time_sent));

    timersub(&time_received, &time_sent, rtt);
    return bytes - ip_header_len;
  }

  return 0;
}

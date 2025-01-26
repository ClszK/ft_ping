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
  icmp->un.echo.sequence = seq;
  icmp->code = 0;

  gettimeofday(time_sent, NULL);

  icmp->checksum = 0;
  icmp->checksum = checksum(packet, payload_size);
}

int receive_ping_reply(int sock, pid_t id, int seq,
                       struct timeval *rtt) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  char buffer[1024];
  ssize_t bytes;

  struct iphdr *ip;
  int ip_header_len;
  struct icmphdr *icmp_reply;

  bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                   (struct sockaddr *)&addr, &addr_len);
  if (bytes < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return 0;
    else {
      perror("recvfrom");
      return -1;
    }
  }

  ip = (struct iphdr *)buffer;
  ip_header_len = ip->ihl * 4;
  icmp_reply = (struct icmphdr *)(buffer + ip_header_len);

  if (icmp_reply->type == ICMP_ECHOREPLY &&
      ntohs(icmp_reply->un.echo.id) == (id & 0xFFFF) &&
      ntohs(icmp_reply->un.echo.sequence) == seq) {
    printf("%ld\n", bytes - ip_header_len);
    struct timeval time_received, time_sent;

    gettimeofday(&time_received, NULL);
    memcpy(&time_sent,
           buffer + ip_header_len + sizeof(struct icmphdr),
           sizeof(time_sent));

    timersub(&time_received, &time_sent, rtt);
    return 1;
  }

  return 0;
}

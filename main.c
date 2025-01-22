#include "ping.h"

int main(int argc, char* argv[]) {
  struct hostent* he = NULL;

  int sock;
  struct sockaddr_in dest;

  char packet[64];
  struct icmphdr* icmp;

  if (argc == 1) {
    fprintf(stderr, "ping: missing host operand\n");
    exit(64);
  }

  if (!is_valid_ipv4(argv[1]) &&
      (he = gethostbyname(argv[1])) == NULL) {
    fprintf(stderr, "ping: unknown host\n");
    exit(1);
  }

  if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) <
      0) {
    perror("Socket creation failed");
    exit(1);
  }

  memset(&dest, 0, sizeof(struct sockaddr_in));
  dest.sin_family = AF_INET;
  if (he) {
    memcpy(&dest.sin_addr.s_addr, he->h_addr_list[0],
           he->h_length);
  } else
    dest.sin_addr.s_addr = inet_addr(argv[1]);

  icmp = (struct icmphdr*)packet;
  memset(packet, 0, sizeof(packet));

  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = htons(getpid());
  icmp->un.echo.sequence = 0;
  icmp->checksum = checksum(packet, sizeof(packet));

  if (sendto(sock, packet, sizeof(packet), 0,
             (struct sockaddr*)&dest, sizeof(dest)) < 0) {
    perror("Sendto Failed");
    exit(1);
  }
  printf("Ping request sent to %s\n", argv[1]);

  char buffer[1024];
  struct sockaddr_in src;
  socklen_t addr_len = sizeof(src);
  if (recvfrom(sock, buffer, sizeof(buffer), 0,
               (struct sockaddr*)&src, &addr_len) < 0) {
    perror("Receive failed");
    return 1;
  }
  printf("Ping reply received from %s\n",
         inet_ntoa(src.sin_addr));

  close(sock);

  struct iphdr* ip = (struct iphdr*)buffer;
  int ip_header_len = ip->ihl * 4;
  struct icmphdr* icmp2 =
      (struct icmphdr*)(buffer + ip_header_len);
  printf("ICMP Type: %d\n", icmp2->type);
  printf("ICMP Code: %d\n", icmp2->code);
  printf("Checksum: 0x%x\n", ntohs(icmp2->checksum));
  printf("ID: %d\n", ntohs(icmp2->un.echo.id));
  printf("Sequence: %d\n", ntohs(icmp2->un.echo.sequence));

  return 0;
}
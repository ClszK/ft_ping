#include "ping.h"

int main(int argc, char* argv[]) {
  int sock;
  struct sockaddr_in dest;

  char packet[64];

  if (argc == 1) error_exit("missing host operand", 64);

  if (resolve_host(argv[1], &dest) == -1)
    error_exit("unknown host", EXIT_FAILURE);

  sock = setup_socket();

  pid_t id = getpid();
  struct timeval rtt;
  int status;
  double rtt_ms;

  while (1) {
    create_icmp_packet(packet, 0, sizeof(packet));
    if (sendto(sock, packet, sizeof(packet), 0,
               (struct sockaddr*)&dest, sizeof(dest)) < 0) {
      perror("Sendto Failed");
      exit(1);
    }
    printf("Ping request sent to %s\n", argv[1]);
    status = receive_ping_reply(sock, id, 0, &rtt);

    switch (status) {
      case 1:
        rtt_ms = rtt.tv_sec * 1000.0 + rtt.tv_usec / 1000.0;
        printf("Received ICMP Echo Reply RTT=%.3f ms\n",
               rtt_ms);
        break;
      case 0:
        printf("Request timeout for imcp\n");
      default:
        break;
    }
    usleep((1000 - rtt_ms) * 1000);
  }

  close(sock);

  return 0;
}
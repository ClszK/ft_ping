#include "ping.h"

#define DATA_SIZE 56

volatile int running = 1;

void handle_sigint(int sig) {
  (void)sig;
  running = 0;
}

int main(int argc, char* argv[]) {
  int sock;
  struct sockaddr_in dest;

  char packet[64];

  if (argc == 1) error_exit("missing host operand", 64);

  if (resolve_host(argv[1], &dest) == -1)
    error_exit("unknown host", EXIT_FAILURE);

  sock = setup_socket();

  signal(SIGINT, handle_sigint);
  printf("PING %s (%s): %d data bytes\n", argv[1],
         inet_ntoa(dest.sin_addr), DATA_SIZE);

  pid_t id = getpid();
  struct timeval rtt;
  int status, ttl;
  double rtt_ms;
  int seq = 0;

  int packets_transmitted = 0, packets_received = 0;
  double min_rtt = 0.0, max_rtt = 0.0, sum_rtt = 0.0,
         sumsq_rtt = 0.0;

  while (running) {
    create_icmp_packet(packet, seq, sizeof(packet));
    if (sendto(sock, packet, sizeof(packet), 0,
               (struct sockaddr*)&dest, sizeof(dest)) < 0) {
      perror("Sendto Failed");
      exit(1);
    }
    packets_transmitted++;
    status = receive_ping_reply(sock, id, seq++, &rtt, &ttl,
                                &dest);

    if (status > 0) {
      packets_received++;
      rtt_ms = rtt.tv_sec * 1000.0 + rtt.tv_usec / 1000.0;

      if (packets_received == 1) {
        min_rtt = max_rtt = rtt_ms;
      } else {
        if (rtt_ms < min_rtt) min_rtt = rtt_ms;
        if (rtt_ms > max_rtt) max_rtt = rtt_ms;
      }
      sum_rtt += rtt_ms;
      sumsq_rtt += rtt_ms * rtt_ms;

      printf(
          "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f "
          "ms\n",
          status, inet_ntoa(dest.sin_addr), seq - 1, ttl,
          rtt_ms);
    } else if (status == 0)
      printf("Request timeout for icmp_seq %d\n", seq - 1);

    double rtt_ms_left =
        rtt.tv_sec * 1000.0 + rtt.tv_usec / 1000.0;
    if (1000 - rtt_ms_left > 0)
      usleep((1000 - rtt_ms_left) * 1000);
  }

  printf("\n--- %s ping statistics ---\n", argv[1]);
  int loss =
      ((packets_transmitted - packets_received) * 100) /
      packets_transmitted;
  printf(
      "%d packets transmitted, %d packets received, %d%% "
      "packet loss\n",
      packets_transmitted, packets_received, loss);

  if (packets_received > 0) {
    double avg = sum_rtt / packets_received;
    double variance =
        (sumsq_rtt / packets_received) - (avg * avg);
    double stddev = sqrt(variance);
    printf(
        "round-trip min/avg/max/stddev = "
        "%.3f/%.3f/%.3f/%.3f ms\n",
        min_rtt, avg, max_rtt, stddev);
  }

  close(sock);

  return 0;
}
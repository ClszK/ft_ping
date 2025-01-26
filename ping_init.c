#include "ping.h"

int resolve_host(const char* host,
                 struct sockaddr_in* dest) {
  struct hostent* he = NULL;

  if (is_valid_ipv4(host))
    dest->sin_addr.s_addr = inet_addr(host);
  else {
    if ((he = gethostbyname(host)) == NULL) return -1;
    memcpy(&dest->sin_addr.s_addr, he->h_addr_list[0],
           he->h_length);
  }
  dest->sin_family = AF_INET;
  return 0;
}

int setup_socket() {
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  struct timeval timeout;

  if (sock < 0) {
    perror("Socket create failed");
    exit(EXIT_FAILURE);
  }

  // recvfrom에서 1초동안 블록킹되고, 1초동안 응답이 없으면
  // -1을 반환. 이를 통해 1초마다 새로운 icmp 패킷을 보낼 수
  // 있게 된다.
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof(timeout)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  return sock;
}
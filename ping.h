#ifndef PING_H
#define PING_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef int bool;

/**
 * @brief 문자열의 ip가 옳은지 판별 함수.
 *
 * @param ip 문자열 ip
 * @return bool
 */
bool is_valid_ipv4(const char* ip);

/**
 * @brief packet의 checksum 계산해주는 함수.
 *
 * @param b
 * @param len
 * @return unsigned short
 */
unsigned short checksum(void* b, int len);
void error_exit(const char* msg, int exit_code);

int setup_socket();
/**
 * @brief 주어진 호스트 이름 or IP를 확인하고
 *
 * @param host
 * @param dest
 * @return int
 */
int resolve_host(const char* host,
                 struct sockaddr_in* dest);

void create_icmp_packet(char* packet, int seq,
                        int payload_size);
int receive_ping_reply(int sock, pid_t id, int seq,
                       struct timeval* rtt);

#endif
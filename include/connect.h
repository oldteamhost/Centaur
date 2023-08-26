/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef CONNECT_H
#define CONNECT_H 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

int send_tcp_connect_packet(const char* ip, int port, const char* message);

#ifdef __cplusplus
}
#endif

#endif

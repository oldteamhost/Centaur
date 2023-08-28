/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef PINGER_H
#define PINGER_H
#include "../network/include/icmp4.h"
#include "../network/include/socks5.h"
#include "status.h"
#include "other.h"

#ifdef __cplusplus
extern "C" {
#endif

struct pinger_options
{
  int delay_ping;

  int icmp_timeout;
  int tcp_timeout;

  int tcp_port;
  int icmp_ttl;
};

double pinger(const char* host, int type, struct pinger_options *po);

#ifdef __cplusplus
}
#endif

#endif

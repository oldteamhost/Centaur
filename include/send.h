/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef SEND_H 
#define SEND_H 
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../network/include/icmp4.h"
#include "../base/include/nescathread.h"
#include "../network/include/ip4.h"
#include "../network/include/socks5.h"
#include "../network/include/connect.h"
#include "status.h"
#include "other.h"
#include "pinger.h"

#define SUCCESS_SEND_MESSAGE 0
#define FAILED_SEND_MESSAGE 1
#define VERBOSE

struct send_options
{
  std::unordered_map<std::string, std::vector<std::string>> id_map;

  int send_port;
  bool non_stop;

  std::string myself;
  std::vector<char> bytes;
};

int __send(const std::string& key, struct send_options *so);

#endif

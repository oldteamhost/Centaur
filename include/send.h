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
#include "icmp4.h"
#include "nescathread.h"
#include "connect.h"
#include "status.h"
#include "other.h"
#include "pinger.h"
#include "socks5.h"

#define OK_SEND 0
#define NO_SEND 1
#define VERBOSE

struct send_options
{
    std::unordered_map<std::string, std::vector<std::string>> id_map;

    int send_port;
    bool non_stop;

    std::string myself;
    std::vector<char> bytes;
};

int
__send(const std::string& key, struct send_options *so);

#endif

/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/pinger.h"
#include "include/other.h"
#include "include/status.h"
#include <stdio.h>

double pinger(const char* host, int type, struct pinger_options *po)
{
    double temp_ping = 9999;
    delay_ms(po->delay_ping);
    switch (type)
    {
        case IP4:
        {
            temp_ping = icmp_ping(host, po->icmp_timeout, 8, 0, 0, po->icmp_ttl);
            break;
        }
        case DNS:
        {
            const char* temp_ip = get_ip_by_dns(host);
            temp_ping = icmp_ping(temp_ip, po->icmp_timeout, 8, 0, 0, po->icmp_ttl);
            break;
        }
        case URL:
        {
            const char* temp_dns = clear_url(host);
            const char* temp_ip = get_ip_by_dns(temp_dns);
            temp_ping = icmp_ping(temp_ip, po->icmp_timeout, 8, 0, 0, po->icmp_ttl);
            break;
        }
        case TOR_URL:
        {
            const char* temp_dns = clear_url(host);
            temp_ping = socks5_tcp_ping(temp_dns, po->tcp_port, "127.0.0.1", 9050, -1, po->tcp_timeout);
            break;
        }
        case TOR_DNS:
        {
            temp_ping = socks5_tcp_ping(host, po->tcp_port, "127.0.0.1", 9050, -1, po->tcp_timeout);
            break;
        }
    }

    return temp_ping;
}

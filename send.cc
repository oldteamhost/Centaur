/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/send.h"
#include "include/socks5.h"
#include "include/status.h"
#include <cstdio>

int __send(const std::string& key, struct send_options *so)
{
#ifdef VERBOSE
    printf("> send: key=%s, port=%d, non-stop=%d\n", key.c_str(), so->send_port, so->non_stop);
#endif

    socks_5_connection connection;
    connection.proxy_host = "127.0.0.1";
    connection.proxy_port = 9050;
    connection.socket = -1;

#ifdef VERBOSE
    printf("> tor: host=%s, port=%d, socket=%d\n\n", connection.proxy_host, connection.proxy_port, connection.socket);
#endif

    std::string myself_edit = so->myself + ";";
    std::vector<std::string> total_hosts = so->id_map[key];
    so->bytes.insert(so->bytes.begin(), myself_edit.begin(), myself_edit.end());

    for (int i = 0; i < total_hosts.size(); i++)
    {
        bool ok_send = false;
        int is = this_is(total_hosts[i].c_str());

#ifdef VERBOSE
        printf("> %d total: host=%s, is=%s, message=%s\n", i, total_hosts[i].c_str(), get_this_is(is), so->bytes.data());
#endif

        struct timespec start_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        if (is == TOR_URL || is == URL || is == I2P_URL) {total_hosts[i] = clear_url(total_hosts[i].c_str());}
        if (is == TOR_DNS || is == TOR_URL)
        {
            connection.target_host = total_hosts[i].c_str();
            connection.target_port = so->send_port;

            if (socks5_connect(&connection))
            {
                so->bytes.push_back('\0');
                if (socks5_send(&connection, so->bytes.data(), strlen(so->bytes.data())))
                {
                    ok_send = true;
                }
                socks5_close(&connection);
            }
        }

        if (is == DNS || is == URL || is == IP4)
        {
            int sent = -1;
            if (is == DNS || is == URL)
            {
                const char* temp_ip = get_ip_by_dns(total_hosts[i].c_str());
                sent = send_tcp_connect_packet(temp_ip, 80, so->bytes.data());
            }
            else
            {
                sent = send_tcp_connect_packet(total_hosts[i].c_str(), 80, so->bytes.data());
            }

            if (sent == 0)
            {
                ok_send = true;
            }
        }

        if (ok_send)
        {
            struct timespec end_time;
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
            printf("\tdata sent successfully at %.2fs in: %s\n",elapsed_time, total_hosts[i].c_str());
            if (!so->non_stop)
            {
                return OK_SEND;
            }
            else
            {
                puts("");
            }
        }
    }
    return NO_SEND;
}

/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
 * Author md5.h - Zunawe
*/

#ifndef OTHER_H
#define OTHER_H 
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int dns_or_ip(const char* host);
const char* clear_url(const char* url);
const char* get_time(void);
const char* get_ip_by_dns(const char* dns);
void delay_ms(int milliseconds);
const char* get_local_ip(void);

#ifdef __cplusplus
}
#endif

#endif

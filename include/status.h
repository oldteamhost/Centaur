/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef STATUS_H
#define STATUS_H
#include <stdio.h>
#include <string.h>
#include "other.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP4     1
#define DNS     2
#define URL     3
#define I2P_DNS 4
#define I2P_URL 5
#define TOR_DNS 6
#define TOR_URL 7

int
search_constchar(const char *sentence, const char *word);

#define EXAMPLE_URL "http://"
#define _EXAMPLE_URL "https://"
#define EXAMPLE_TOR ".onion"
#define EXAMPLE_I2P ".i2p"

int
this_is(const char* node);

void
print_this_is(const char* node);

const char*
get_this_is(int type);

#ifdef __cplusplus
}
#endif

#endif

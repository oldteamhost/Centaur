/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/status.h"
#include "include/other.h"

int search_constchar(const char *sentence, const char *word)
{
    if (strstr(sentence, word) != NULL) {return 0;} 
    else {return -1;}
}

int
this_is(const char* node)
{
    int this_is_url = 0;
    if (search_constchar(node, EXAMPLE_URL) == 0 || search_constchar(node, _EXAMPLE_URL) == 0)
    {
        this_is_url = 1;
    }

    if (search_constchar(node, EXAMPLE_TOR) == 0)
    {
        if (this_is_url)
        {
            return TOR_URL;
        }
        else
        {
            return TOR_DNS;
        }
    }

    if (search_constchar(node, EXAMPLE_I2P) == 0)
    {
        if (this_is_url)
        {
            return I2P_URL;
        }
        else
        {
            return I2P_DNS;
        }
    }

    if (this_is_url)
    {
        return URL;
    }

    if (dns_or_ip(node) == 1)
    {
        return DNS;
    }
    else
    {
        return IP4;
    }

    return -1;
}

const char*
get_this_is(int type)
{
    switch (type)
    {
        case IP4:
            return "IPv4";
        case DNS:
            return "DNS";
        case URL:
            return "URL";
        case TOR_DNS:
            return "TOR_DNS";
        case TOR_URL:
            return "TOR_URL";
        case I2P_DNS:
            return "I2P_DNS";
        case I2P_URL:
            return "I2P_URL";
    }

    return "-1";
}

void
print_this_is(const char* node)
{
    int type = this_is(node);
    printf("(%s) this is: %s\n", node, get_this_is(type));
}

/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef CENTAUR_H 
#define CENTAUR_H 
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <unordered_map>

#include "file.h"
#include "getopt.h"
#include "other.h"
#include "icmp4.h"
#include "socks5.h"

#define VERSION "0.2"

struct centaur_data_md5
{
    std::vector<std::string> temp_file;
    std::string combined_values; /* 192.168.1.1;192.168.1.2 */
    std::string key; /* combined_values in encode md5 */
    std::unordered_map<std::string, std::vector<std::string>> md5_hash_map;
};

const struct option longopts[] =
{
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {"message", required_argument, 0, 'm'},
    {"port", required_argument, 0, 'p'},
    {"ping-threads", required_argument, 0, 1},
    {"ping-delay", required_argument, 0, 2},
    {"icmp-timeout", required_argument, 0, 3},
    {"tcp-timeout", required_argument, 0, 4},
    {"ping-port", required_argument, 0, 5},
    {"ttl", required_argument, 0, 6},
    {"non-stop", no_argument, 0, 7},
    {"myself", required_argument, 0, 8}
};

void usage(const char* run);
void version(void);
void sudorun(void);
std::vector<char> const_char_ptr_to_vector(const char* data);
void print_char_vector(const std::vector<char>& vec);
std::vector<char> std_string_to_vector(const std::string& data);
std::vector<char> char_ptr_to_vector(char* data);
template <std::size_t N> std::vector<char>
char_array_to_vector(const char (&data)[N]);
void parse_file(centaur_data_md5 *cd5, std::string& filename);
std::vector<std::string> split_string_string(const std::string& str, char delimiter);
void encode_md5_data(centaur_data_md5 *cd5);
void print_md5_hash(const uint8_t* md5hash);

#endif

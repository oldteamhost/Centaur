/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef RECEIVE_H
#define RECEIVE_H
#include <iostream>
#include <vector>
#include <unordered_map>

struct receive_base
{
  std::unordered_map<std::string, std::vector<std::string>> id_map;
};

struct receive_options
{
  int timeout_ms;
  int source_port;
  std::vector<char> bytes;
  receive_base rb;
};

std::string extract_myself(std::vector<char>& bytes);
int __receive(const std::string& key, struct receive_options *ro);

#endif


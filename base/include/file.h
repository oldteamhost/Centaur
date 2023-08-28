/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef FILE_H
#define FILE_H
#include <vector>
#include <iostream>
#include <fstream>

std::vector<std::string> read_file(const std::string& filename);
int write_line(std::string path, std::string line);

#endif

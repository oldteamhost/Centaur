/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/file.h"

std::vector<std::string> read_file(const std::string& filename)
{
    std::string line;
    std::vector<std::string> lines;

    std::ifstream file(filename);
	if (file.fail())
    {
        return {"-1"};
    }

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    return lines;
}

int write_line(std::string path, std::string line)
{
    std::ofstream outfile;
    outfile.open(path, std::ios_base::app);
    if (!outfile.is_open()){return -1;}
    outfile << line;
    if (outfile.fail()){return -2;}
    outfile.close();
    if (outfile.fail()){return -3;}
    return 0;
}

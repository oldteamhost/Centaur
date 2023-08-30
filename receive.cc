/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/receive.h"
#include <algorithm>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <utility>
#include <vector>

std::string extract_myself(std::vector<char>& bytes)
{
  std::string result = "";
  bool found = false;

  for (char c : bytes) {
    if (c == ';') {
      found = true;
      break;
    }
    result.push_back(c);
  }

  if (found) {
    bytes.erase(bytes.begin(), bytes.begin() + result.size() + 1);
  }
  else {
    bytes.clear();
  }

  return result;
}

int __receive(const std::string& key, struct receive_options *ro)
{
  std::string self = extract_myself(ro->bytes);
  printf("> receive: key=%s, sport=%d\n", key.c_str(), ro->source_port);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    return 1;
  }
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(ro->source_port);
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(fd, (struct sockaddr*)&server, sizeof(server)) == -1) {
    perror("bind");
    close(fd);
    return 1;
  }

  if (listen(fd, 5) == -1) {
    std::cerr << "listen fuck;";
    close(fd);
    return 1;
  }
  std::cout << "Listening for connections..." << std::endl;

  while (true) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
      std::cerr << "Error accepting connection" << std::endl;
      continue;
    }

    std::cout << "Connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

    char buffer[4096];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
      std::cerr << "Error receiving data" << std::endl;
      close(clientSocket);
      continue;
    }

    std::string message(buffer, bytesRead);
    ro->rb.id_map[self].push_back(message);
    printf("ip=%s message=%s\n", inet_ntoa(clientAddress.sin_addr), message.c_str());

    for (const auto& entry : ro->rb.id_map) {
        const std::string& key = entry.first;
        const std::vector<std::string>& values = entry.second;

        std::cout << "Key: " << key << ", Values: ";
        for (const std::string& value : values) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    close(clientSocket);
  }
  close(fd);

  return 0;
}

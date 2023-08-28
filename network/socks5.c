/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/socks5.h"

bool socks5_connect(socks_5_connection *connection)
{
  struct sockaddr_in proxy_addr;
  connection->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (connection->socket == -1) {
    perror("Socket creation failed");
    return false;
  }

  proxy_addr.sin_family = AF_INET;
  proxy_addr.sin_port = htons(connection->proxy_port);
  proxy_addr.sin_addr.s_addr = inet_addr(connection->proxy_host);

  if (connect(connection->socket, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) == -1) {
    perror("Proxy connection failed");
    return false;
  }

  if (!socks5_handshake(connection)) {
    perror("SOCKS5 handshake failed");
    return false;
  }

  if (!socks5_send_command(connection)) {
    perror("SOCKS5 command sending failed");
    return false;
  }

  if (!socks5_verify_response(connection)) {
    perror("SOCKS5 command response failed");
    return false;
  }

  return true;
}

bool socks5_send(socks_5_connection *connection, const char *data, size_t size)
{
  if (send(connection->socket, data, size, 0) == -1) {
    perror("Data sending failed");
    return false;
  }
  return true;
}

void socks5_close(socks_5_connection *connection)
{
  close(connection->socket);
}

bool socks5_handshake(socks_5_connection *connection)
{
  unsigned char handshake[] = {0x05, 0x01, 0x00};
  if (send(connection->socket, handshake, sizeof(handshake), 0) == -1) {
    perror("Handshake sending failed");
    return false;
  }

  unsigned char handshake_response[2];
  if (recv(connection->socket, handshake_response, sizeof(handshake_response), 0) == -1) {
      perror("Handshake response receiving failed");
      return false;
  }

  if (handshake_response[0] != 0x05 || handshake_response[1] != 0x00) {
    fprintf(stderr, "Handshake failed\n");
    return false;
  }

  return true;
}

bool socks5_send_command(socks_5_connection *connection)
{
  unsigned char connect_command[256]; /* Просто задаем достаточный размер для буфера */
  size_t connect_command_len = 0;

  connect_command[connect_command_len++] = 0x05; /* SOCKS version */
  connect_command[connect_command_len++] = 0x01; /* Connect command */
  connect_command[connect_command_len++] = 0x00; /* Reserved */
  connect_command[connect_command_len++] = 0x03; /* Domain name type */
  connect_command[connect_command_len++] = (unsigned char)strlen(connection->target_host);

  memcpy(connect_command + connect_command_len, connection->target_host, strlen(connection->target_host));
  connect_command_len += strlen(connection->target_host);

  unsigned short target_port = htons(connection->target_port);
  memcpy(connect_command + connect_command_len, &target_port, sizeof(target_port));
  connect_command_len += sizeof(target_port);

  if (send(connection->socket, connect_command, connect_command_len, 0) == -1) {
    perror("Connect command sending failed");
    return false;
  }

  return true;
}

bool socks5_verify_response(socks_5_connection *connection)
{
  unsigned char response[10];
  if (recv(connection->socket, response, sizeof(response), 0) == -1) {
    perror("Response receiving failed");
    return false;
  }

  if (response[0] != 0x05 || response[1] != 0x00 || response[2] != 0x00) {
    fprintf(stderr, "Connection failed\n");
    return false;
  }

  return true;
}

#include <sys/time.h>
#include <errno.h>
double socks5_tcp_ping(const char* dest_ip, int port, const char* proxy_host, int proxy_port, int socket, int timeout_ms)
{
  socks_5_connection connection;
  connection.proxy_host = proxy_host;
  connection.socket = socket;
  connection.target_port = port;
  connection.proxy_port = proxy_port;
  connection.target_host = dest_ip;

  struct timeval start_time, end_time;
  double response_time = -1.0;

  struct timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;
  setsockopt(connection.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

  if (socks5_connect(&connection)) {
    if (socks5_send(&connection, "ping", strlen("ping"))) {
      char response_buffer[4096];
      gettimeofday(&start_time, NULL);
      ssize_t bytes_received = recv(connection.socket, response_buffer, sizeof(response_buffer), 0);
      gettimeofday(&end_time, NULL);
      if (bytes_received > 0) {
        response_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_usec - start_time.tv_usec) / 1000.0;
      }
      if (bytes_received == -1){
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
          printf("Timeout waiting for data\n");
        }
        else {
          perror("Error receiving data");
        }
      }
      else if (bytes_received == 0){
          printf("Connection closed by remote host\n");
      }

      timeout.tv_sec = 0;
      timeout.tv_usec = 0;
      setsockopt(connection.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    }
    socks5_close(&connection);
  }
  return response_time;
}

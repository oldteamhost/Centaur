#include "include/connect.h"

int send_tcp_connect_packet(const char* ip, int port, const char* message)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        close(sock);
        return -1;
    }

    size_t len = strlen(message);
    if (send(sock, message, len, 0) != (ssize_t)len)
    {
        close(sock);
        return -1;
    }

    close(sock);
    return 0;
}

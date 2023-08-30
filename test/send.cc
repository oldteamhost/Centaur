#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>

void sendPacket(const char* message) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(33010);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to the server." << std::endl;
        perror("conenct");
        close(sock);
        return;
    }

    if (send(sock, message, strlen(message), 0) == -1) {
        std::cerr << "Failed to send packet." << std::endl;
    } else {
        std::cout << "Packet sent successfully." << std::endl;
    }

    close(sock);
}

int main() {
    const char* message = "Hello, TCP Server!";  // Сообщение для отправки

    int numPackets = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < numPackets; ++i) {
        threads.emplace_back(sendPacket, message);
    }

    for (auto& thread : threads) {
        thread.join();
    }


    return 0;
}


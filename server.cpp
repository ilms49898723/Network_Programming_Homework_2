#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "global.h"
#include "nputility.h"

constexpr int MAXN = 2048;

void serverFunc(const int& fd);

int main(int argc, char const** argv) {
    // check arguments
    if (argc != 2) {
        fprintf(stderr, "usage %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // read port
    int port;
    if (sscanf(argv[1], "%d", &port) != 1) {
        fprintf(stderr, "port %s is not valid\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // socket initialize
    // socketfd, sockaddr_in
    int socketfd;
    sockaddr_in serverAddr;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    // bind
    bind(socketfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    // set timeout
    setSocketTimeout(socketfd, 0, 200);
    // run server function
    serverFunc(socketfd);
    return 0;
}

void serverFunc(const int& fd) {
    // fd_set initialize
    fd_set fdset;
    int maxfdp1 = fd + 1;
    FD_ZERO(&fdset);
    // client sockaddr_in and its (sockaddr*)&sockaddr_in
    sockaddr_in clientAddr;
    sockaddr* clientAddrp = reinterpret_cast<sockaddr*>(&clientAddr);
    // client socket_len
    socklen_t clientLen;
    // while loop to select
    for ( ; ; ) {
        // set socket fd
        FD_SET(fd, &fdset);
        printf("after select\n");
        int nready = select(maxfdp1, &fdset, NULL, NULL, NULL);
        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                fprintf(stderr, "select: %s\n", strerror(errno));
            }
        }
        if (FD_ISSET(fd, &fdset)) {
            // TODO: complete it
            char buffer[MAXN];
            clientLen = sizeof(clientAddr);
            memset(buffer, 0, sizeof(buffer));
            int n, m;
            printf("wait recvfrom\n");
            m = recvfrom(fd, buffer, MAXN, 0, clientAddrp, &clientLen);
            strcpy(buffer, "WELCOME!!!");
            printf("send %s to client\n", buffer);
            n = sendto(fd, buffer, strlen(buffer), 0, clientAddrp, clientLen);
            printf("send complete!\n");
        }
    }
}


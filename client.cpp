#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "global.h"
#include "nputility.h"

void clientFunc(const int& fd, sockaddr_in serverAddr);

int main(int argc, char const** argv) {
    // check argments
    if (argc != 3) {
        fprintf(stderr, "usage %s <server address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // check port
    int port;
    if (sscanf(argv[2], "%d", &port) != 1) {
        fprintf(stderr, "%s is not valid\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    // socket initialize
    int socketfd;
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    // set socket timeout
    setSocketTimeout(socketfd, 0, 200);
    // run clientFunc
    clientFunc(socketfd, serverAddr);
    return 0;
}

void clientFunc(const int& fd, sockaddr_in serverAddr) {
    // fd_set initialize
    fd_set fdset;
    int maxfdp1 = fd + 1;
    FD_ZERO(&fdset);
    // server sockaddr*
    sockaddr* serverAddrp = reinterpret_cast<sockaddr*>(&serverAddr);
    // loop to select
    for ( ; ; ) {
        // set socket fd
        FD_SET(fd, &fdset);
        FD_SET(fileno(stdin), &fdset);
        int nready = select(maxfdp1, &fdset, NULL, NULL, NULL);
        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                fprintf(stderr, "select: %s\n", strerror(errno));
            }
        }
        if (FD_ISSET(fileno(stdin), &fdset)) {
            // TODO: complete it
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, MAXN, stdin) == NULL) {
                continue;
            }
            trimNewLine(buffer);
            udpSendTo(fd, buffer, strlen(buffer), serverAddrp);
            udpRecvFrom(fd, buffer, MAXN, serverAddrp);
            printf("recv %s from server\n", buffer);
        }
    }
}


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "global.h"

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
    // variables definition
    int socketfd;
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    return 0;
}

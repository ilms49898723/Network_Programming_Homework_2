#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "global.h"

int main(int argc, char const** argv) {
    // check arguments
    if (argc != 2) {
        fprintf(stderr, "usage %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // check port
    int port;
    if (sscanf(argv[1], "%d", &port) != 1) {
        fprintf(stderr, "port %s is not valid\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // variables definition
    int socketfd;
    sockaddr_in serverAddr, clientAddr;
    // socket initialize
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    // bind
    bind(socketfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    return 0;
}


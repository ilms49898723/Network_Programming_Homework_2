#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <vector>
#include "global.h"
#include "nputility.h"
#include "udpmessage.h"

// store user data like password, name, birthday, etc.
class UserData {
    public:
        UserData() {
            password = "";
            name = "";
            birthday = "";
            friends.clear();
            registerDate = time(NULL);
            lastLogin = time(NULL);
        }

        virtual ~UserData() {

        }

    public:
        std::string password;
        std::string name;
        std::string birthday;
        std::vector<std::string> friends;
        time_t registerDate;
        time_t lastLogin;
};

// map for Account --> UserData
std::map<std::string, UserData> serverData;

class ServerUtility {
    public:
        static void udpRegister(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            char password[MAXN];
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            sscanf(msg.c_str(), "%*s%s%s", account, password);
            if (serverData.count(account)) {
                snprintf(buffer, MAXN, "Account %s already exists\n", account);
            }
            else {
                serverData[account].password = password;
                serverData[account].registerDate = time(NULL);
                serverData[account].lastLogin = time(NULL);
                snprintf(buffer, MAXN, "Register Success\n");
                printf("New Account %s added\n", account);
            }
            udpSendTo(fd, buffer, strlen(buffer), clientAddrp);
        }
        static void udpLogin(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            char password[MAXN];
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            sscanf(msg.c_str(), "%*s%s%s", account, password);
            if (!serverData.count(account) || serverData[account].password != password) {
                snprintf(buffer, MAXN, "Invalid account or password\n");
            }
            else {
                snprintf(buffer, MAXN, "\nLogin Success!\n\nWelcome %s\nLast Login %s\n",
                        serverData[account].name == "" ? account : serverData[account].name.c_str(),
                        asctime(localtime(&serverData[account].lastLogin)));
                serverData[account].lastLogin = time(NULL);
                printf("Account %s login at %s",
                        account, asctime(localtime(&serverData[account].lastLogin)));
            }
            udpSendTo(fd, buffer, strlen(buffer), clientAddrp);
        }
};

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
        fprintf(stderr, "port %s is not a valid port number\n", argv[1]);
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
    // while loop to select
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
            char buffer[MAXN];
            if (fgets(buffer, MAXN, stdin) == NULL) {
                continue;
            }
            trimNewLine(buffer);
            if (std::string(buffer) == "QUIT") {
                return;
            }
        }
        if (FD_ISSET(fd, &fdset)) {
            // TODO: complete it
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            udpRecvFrom(fd, buffer, MAXN, clientAddrp);
            std::string msg = buffer;
            if (msg == msgNEWCONNECTION) {
                snprintf(buffer, MAXN, "WELCOME!\n");
                udpSendTo(fd, buffer, strlen(buffer), clientAddrp);
            }
            else if (msg.find(msgREGISTER) == 0u) {
                ServerUtility::udpRegister(fd, clientAddrp, msg);
            }
            else if (msg.find(msgLOGIN) == 0u) {
                ServerUtility::udpLogin(fd, clientAddrp, msg);
            }
        }
    }
}


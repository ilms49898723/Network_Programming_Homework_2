#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <vector>
#include "global.h"
#include "nputility.h"
#include "udpmessage.h"
#include "UDPUtil.h"

UDPUtil udp;

// store user data like password, name, birthday, etc.
class UserData {
    public:
        UserData() {
            isOnline = false;
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
        bool isOnline;
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
            udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
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
                serverData[account].isOnline = true;
                serverData[account].lastLogin = time(NULL);
                printf("Account %s login at %s",
                        account, asctime(localtime(&serverData[account].lastLogin)));
            }
            udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
        }

        static void udpLogout(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            sscanf(msg.c_str(), "%*s%s", account);
            snprintf(buffer, MAXN, "\nLogout Success!\n\n");
            serverData[account].isOnline = false;
            serverData[account].lastLogin = time(NULL);
            printf("Account %s logout at %s", account, asctime(localtime(&serverData[account].lastLogin)));
            udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
        }

        static void udpShowProfile(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            sscanf(msg.c_str(), "%*s%s", account);
            std::string regDate = asctime(localtime(&serverData[account].registerDate));
            std::string lastDate = asctime(localtime(&serverData[account].lastLogin));
            std::string toSend = std::string("Account: ") + account + "\n" +
                                 std::string("Name: ") + serverData[account].name + "\n" +
                                 std::string("Birthday: ") + serverData[account].birthday + "\n" +
                                 std::string("Register Date: ") + regDate +
                                 std::string("Last Login: ") + lastDate;
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
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
            udp.udpRecv(fd, clientAddrp, buffer, MAXN);
            std::string msg = buffer;
            if (msg == msgNEWCONNECTION) {
                snprintf(buffer, MAXN, "WELCOME!\n");
                udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
            }
            else if (msg.find(msgREGISTER) == 0u) {
                ServerUtility::udpRegister(fd, clientAddrp, msg);
            }
            else if (msg.find(msgLOGIN) == 0u) {
                ServerUtility::udpLogin(fd, clientAddrp, msg);
            }
            else if (msg.find(msgLOGOUT) == 0u) {
                ServerUtility::udpLogout(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSHOWPROFILE) == 0u) {
                ServerUtility::udpShowProfile(fd, clientAddrp, msg);
            }
        }
    }
}


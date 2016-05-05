#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "global.h"
#include "nputility.h"
#include "udpmessage.h"
#include "UDPUtil.h"

NPStage nowStage;
UDPUtil udp;

std::string nowAccount;

class ClientUtility {
    public:
        static void udpRegister(const int& fd, sockaddr*& serverAddrp) {
            char account[MAXN];
            char password[MAXN];
            char buffer[MAXN];
            printf("Account: ");
            if (fgets(account, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(account);
            if (!isValidString(account)) {
                fprintf(stderr, "Account may not contain space or tab character\n");
                return;
            }
            printf("Password: ");
            if (fgets(password, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n");
                return;
            }
            std::string msg = msgREGISTER + " " + account + " " + password;
            udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length());
            printf("%s\n", buffer);
        }

        static void udpLogin(const int& fd, sockaddr*& serverAddrp) {
            char account[MAXN];
            char password[MAXN];
            char buffer[MAXN];
            printf("Account: ");
            if (fgets(account, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(account);
            if (!isValidString(account)) {
                fprintf(stderr, "Account may not contain space or tab character\n");
                return;
            }
            printf("Password: ");
            if (fgets(password, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n");
                return;
            }
            std::string msg = msgLOGIN + " " + account + " " + password;
            udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length());
            printf("%s\n", buffer);
            if (std::string(buffer).find("\nLogin Success!\n\n") != std::string::npos) {
                nowStage = NPStage::MAIN;
                nowAccount = account;
            }
        }

        static void udpLogout(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            std::string msg = msgLOGOUT + " " + nowAccount;
            udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length());
            printf("%s\n", buffer);
            if (std::string(buffer).find("\nLogout Success!\n\n") != std::string::npos) {
                nowStage = NPStage::WELCOME;
                nowAccount = "";
            }
        }

        static void udpShowProfile(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            std::string msg = msgSHOWPROFILE + " " + nowAccount;
            printf("nowAccount %s\n", nowAccount.c_str());
            udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length());
            printf("%s\n", buffer);
        }

        static void udpSetProfile(const int& fd, sockaddr*& serverAddrp) {
            char name[MAXN];
            char birthday[MAXN];
            printf("Name: ");
            if (fgets(name, MAXN, stdin) == NULL || !isValidString(name)) {
                return;
            }
            trimNewLine(name);
            printf("Birthday: ");
            if (fgets(birthday, MAXN, stdin) == NULL || !isValidString(birthday)) {
                return;
            }
            trimNewLine(birthday);
            char buffer[MAXN];
            std::string msg = msgSETPROFILE + " " + nowAccount + " " + name + " " + birthday;
            udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length());
            printf("%s\n", buffer);
        }

    private:
        static bool isValidString(const std::string& str) {
            for (char c : str) {
                if (c == ' ' || c == '\t') {
                    return false;
                }
            }
            for (char c : str) {
                if (c != '\n') {
                    return true;
                }
            }
            return false;
        }
};

void clientFunc(const int& fd, sockaddr_in serverAddr);
void printMessage(const NPStage& stage);

int main(int argc, char const** argv) {
    // check argments
    if (argc != 3) {
        fprintf(stderr, "usage %s <server address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // check port
    int port;
    if (sscanf(argv[2], "%d", &port) != 1) {
        fprintf(stderr, "%s is not a valid port number\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    nowAccount = "";
    // socket initialize
    int socketfd;
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) < 0) {
        fprintf(stderr, "inet_pton: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    // set socket timeout
    setSocketTimeout(socketfd, 0, 200);
    // run clientFunc
    clientFunc(socketfd, serverAddr);
    return 0;
}

void clientFunc(const int& fd, sockaddr_in serverAddr) {
    nowStage = NPStage::INIT;
    char buffer[MAXN];
    // fd_set initialize
    fd_set fdset;
    int maxfdp1 = fd + 1;
    FD_ZERO(&fdset);
    // server sockaddr*
    sockaddr* serverAddrp = reinterpret_cast<sockaddr*>(&serverAddr);
    udp.udpTrans(fd, serverAddrp, buffer, MAXN, msgNEWCONNECTION.c_str(), msgNEWCONNECTION.length());
    printf("%s\n", buffer);
    nowStage = NPStage::WELCOME;
    printMessage(nowStage);
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
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, MAXN, stdin) == NULL) {
                continue;
            }
            trimNewLine(buffer);
            std::string command = buffer;
            switch (static_cast<int>(nowStage)) {
                case 0:
                    fprintf(stderr, "Invalid Command\n");
                    break;
                case 1:
                    if (command.find("Q") == 0u) {
                        return;
                    }
                    else if (command.find("L") == 0u) {
                        ClientUtility::udpLogin(fd, serverAddrp);
                    }
                    else if (command.find("R") == 0u) {
                        ClientUtility::udpRegister(fd, serverAddrp);
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 2:
                    if (command.find("L") == 0u) {
                        ClientUtility::udpLogout(fd, serverAddrp);
                    }
                    else if (command.find("SP") == 0u) {
                        ClientUtility::udpShowProfile(fd, serverAddrp);
                    }
                    else if (command.find("SE") == 0u) {
                        ClientUtility::udpSetProfile(fd, serverAddrp);
                    }
                    break;
            }
            printMessage(nowStage);
        }
    }
}

void printMessage(const NPStage& stage) {
    switch (static_cast<int>(stage)) {
        case 0:
            break;
        case 1:
            printf("%s~ ", msgOptWELCOME.c_str());
            break;
        case 2:
            printf("%s~ ", msgOptMAIN.c_str());
            break;
    }
    fflush(stdout);
}


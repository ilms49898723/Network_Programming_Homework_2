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
int nowArticleIndex;

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
            strcpy(password, getpass("Password: "));
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n");
                return;
            }
            std::string msg = msgREGISTER + " " + account + " " + password;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
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
            strcpy(password, getpass("Password: "));
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n");
                return;
            }
            std::string msg = msgLOGIN + " " + account + " " + password;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer).find("Login Success!\n\n") != std::string::npos) {
                nowStage = NPStage::MAIN;
                nowAccount = account;
            }
        }

        static void udpLogout(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            std::string msg = msgLOGOUT + " " + nowAccount;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer).find("Logout Success!\n\n") != std::string::npos) {
                nowStage = NPStage::WELCOME;
                nowAccount = "";
            }
        }

        static void udpDeleteAccount(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            printf("ARE YOU SURE?(yes/no): ");
            if (fgets(buffer, MAXN, stdin) == NULL) {
                printf("CANCELED\n\n");
                return;
            }
            trimNewLine(buffer);
            if (std::string(buffer) == "yes") {
                std::string msg = msgDELETEACCOUNT + " " + nowAccount;
                if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                    return;
                }
                printf("\n%s\n", buffer);
                nowStage = NPStage::WELCOME;
                nowAccount = "";
            }
            else {
                printf("CANCELED\n\n");
            }
        }

        static void udpShowProfile(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            std::string msg = msgSHOWPROFILE + " " + nowAccount;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
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
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpAddArticle(const int& fd, sockaddr*& serverAddrp) {
            // format: ADDARTICLE 0 account viewerType [viewers]
            // server return article index
            // format: ADDARTICLE 1 index title
            // server return article index
            // format: ADDARTICLE 2 index content
            // server return SUCCESS MSG
            std::string msg;
            msg = msgADDARTICLE + " 0 " + nowAccount;
            char buffer[MAXN];
            printf("Set permission(1:public, 2:author, 3:friend, 4:specific): ");
            while (true) {
                if (fgets(buffer, MAXN, stdin) == NULL) {
                    return;
                }
                trimNewLine(buffer);
                if (std::string(buffer) == "1") {
                    msg = msg + " " + buffer;
                    break;
                }
                else if (std::string(buffer) == "2") {
                    msg = msg + " " + buffer;
                    break;
                }
                else if (std::string(buffer) == "3") {
                    msg = msg + " " + buffer;
                    break;
                }
                else if (std::string(buffer) == "4") {
                    msg = msg + " " + buffer;
                    printf("Please enter account(one per line) who can view this article\nPress ^D to finish\n");
                    char viewAccount[MAXN];
                    while (fgets(viewAccount, MAXN, stdin) != NULL) {
                        msg = msg + " " + viewAccount;
                    }
                    break;
                }
                else {
                    printf("Please enter number between 1 to 4: ");
                }
            }
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            int articleIndex;
            sscanf(buffer, "%d", &articleIndex);
            printf("Article Index Number is %d\n", articleIndex);
            char title[MAXN];
            printf("Title: ");
            if (fgets(title, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(title);
            msg = msgADDARTICLE + " 1 " + std::to_string(articleIndex) + " " + title;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            msg = msgADDARTICLE + " 2 " + std::to_string(articleIndex) + " ";
            char content[MAXN];
            printf("Content(press ^D to finish):\n");
            while (fgets(content, MAXN, stdin) != NULL) {
                msg = msg + content;
            }
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpEditArticle(const int& fd, sockaddr*& serverAddrp) {
            // format: EDITARTICLE 0 index viewType [viewers]
            // server return result
            // format: EDITARTICLE 1 index title
            // server return result
            // format: EDITARTICLE 2 index content
            // server return result
            std::string msg;
            char buffer[MAXN];
            int index = nowArticleIndex;
            msg = msgCHECKARTICLEPERMISSION + " " + nowAccount + " " + std::to_string(index);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            if (std::string(buffer) == msgPERMISSIONDENIED) {
                printf("\n%s\n", buffer);
                return;
            }
            while (true) {
                char command[MAXN];
                printf("%s~ ", msgOptEDITARTICLE.c_str());
                if (fgets(command, MAXN, stdin) == NULL) {
                    break;
                }
                trimNewLine(command);
                if (std::string(command) == "P") {
                    printf("Set permission(1:public, 2:author, 3:friend, 4:specific): ");
                    msg = msgEDITARTICLE + " 0 " + std::to_string(index);
                    while (true) {
                        if (fgets(buffer, MAXN, stdin) == NULL) {
                            return;
                        }
                        trimNewLine(buffer);
                        if (std::string(buffer) == "1") {
                            msg = msg + " " + buffer;
                            break;
                        }
                        else if (std::string(buffer) == "2") {
                            msg = msg + " " + buffer;
                            break;
                        }
                        else if (std::string(buffer) == "3") {
                            msg = msg + " " + buffer;
                            break;
                        }
                        else if (std::string(buffer) == "4") {
                            msg = msg + " " + buffer;
                            printf("Please enter account(one per line) who can view this article\n");
                            printf("Press ^D to finish\n");
                            char viewAccount[MAXN];
                            while (fgets(viewAccount, MAXN, stdin) != NULL) {
                                msg = msg + " " + viewAccount;
                            }
                            break;
                        }
                        else {
                            printf("Please enter number between 1 to 4: ");
                        }
                    }
                    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                        return;
                    }
                    printf("\n%s\n", buffer);
                }
                else if (std::string(command) == "T") {
                    msg = msgEDITARTICLE + " 1 " + std::to_string(index);
                    char title[MAXN];
                    printf("Title: ");
                    if (fgets(title, MAXN, stdin) == NULL) {
                        return;
                    }
                    trimNewLine(title);
                    msg = msg + " " + title;
                    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                        return;
                    }
                    printf("\n%s\n", buffer);
                }
                else if (std::string(command) == "C") {
                    msg = msgEDITARTICLE + " 2 " + std::to_string(index) + " ";
                    char content[MAXN];
                    printf("Content(Press ^D to finish):\n");
                    while (fgets(content, MAXN, stdin) != NULL) {
                        msg += content;
                    }
                    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                        return;
                    }
                    printf("\n%s\n", buffer);
                }
                else if (std::string(command) == "Q") {
                    break;
                }
                else {
                    printf("Command not found!\n");
                }
            }
            msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(index);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpDeleteArticle(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgDELETEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgPERMISSIONDENIED) {
                return;
            }
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
        }

        static void udpShowArticle(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgSHOWARTICLE + " " + nowAccount;
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpEnterArticle(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            int index;
            printf("Article Index: ");
            while (true) {
                if (fgets(buffer, MAXN, stdin) == NULL) {
                    return;
                }
                if (sscanf(buffer, "%d", &index) == 1) {
                    break;
                }
            }
            std::string msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(index);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgPERMISSIONDENIED) {
                return;
            }
            nowArticleIndex = index;
            nowStage = NPStage::ARTICLE;
        }

        static void udpLikeArticle(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgLIKEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgARTICLENOTFOUND) {
                nowArticleIndex = -1;
                nowStage = NPStage::MAIN;
                return;
            }
            msg = msgENTERARTICLE + " " + std::to_string(nowArticleIndex);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpUnlikeArticle(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgUNLIKEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgARTICLENOTFOUND) {
                nowArticleIndex = -1;
                nowStage = NPStage::MAIN;
                return;
            }
            msg = msgENTERARTICLE + " " + std::to_string(nowArticleIndex);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpCommentArticle(const int& fd, sockaddr*& serverAddrp) {
            // format: COMMENTARTICLE account index message
            char buffer[MAXN];
            printf("Comment: ");
            if (fgets(buffer, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(buffer);
            std::string msg = msgCOMMENTARTICLE + " " + nowAccount + " " +
                              std::to_string(nowArticleIndex) + " " + buffer;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgARTICLENOTFOUND) {
                nowArticleIndex = -1;
                nowStage = NPStage::MAIN;
                return;
            }
            msg = msgENTERARTICLE + " " + std::to_string(nowArticleIndex);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpEditCommentArticle(const int& fd, sockaddr*& serverAddrp) {
            // format: EDITCOMMENTARTICLE account index message
            char buffer[MAXN];
            printf("Comment: ");
            if (fgets(buffer, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(buffer);
            std::string msg = msgEDITCOMMENTARTICLE + " " + nowAccount + " " +
                              std::to_string(nowArticleIndex) + " " + buffer;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgARTICLENOTFOUND) {
                nowArticleIndex = -1;
                nowStage = NPStage::MAIN;
                return;
            }
            msg = msgENTERARTICLE + " " + std::to_string(nowArticleIndex);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpDeleteCommentArticle(const int& fd, sockaddr*& serverAddrp) {
            // format DELETECOMMENTARTICLE account index
            std::string msg = msgDELETECOMMENTARTICLE + " " + nowAccount + " " +
                              std::to_string(nowArticleIndex);
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            if (std::string(buffer) == msgARTICLENOTFOUND) {
                nowArticleIndex = -1;
                nowStage = NPStage::MAIN;
                return;
            }
            msg = msgENTERARTICLE + " " + std::to_string(nowArticleIndex);
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
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
    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msgNEWCONNECTION.c_str(), msgNEWCONNECTION.length()) < 0) {
        return;
    }
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
                case 0: // init
                    fprintf(stderr, "Invalid Command\n");
                    break;
                case 1: // welcome
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
                case 2: // main
                    if (command.find("L") == 0u) {
                        ClientUtility::udpLogout(fd, serverAddrp);
                    }
                    else if (command.find("DA") == 0u) {
                        ClientUtility::udpDeleteAccount(fd, serverAddrp);
                    }
                    else if (command.find("SP") == 0u) {
                        ClientUtility::udpShowProfile(fd, serverAddrp);
                    }
                    else if (command.find("SE") == 0u) {
                        ClientUtility::udpSetProfile(fd, serverAddrp);
                    }
                    else if (command.find("SA") == 0u) {
                        ClientUtility::udpShowArticle(fd, serverAddrp);
                    }
                    else if (command.find("A") == 0u) {
                        ClientUtility::udpAddArticle(fd, serverAddrp);
                    }
                    else if (command.find("E") == 0u) {
                        ClientUtility::udpEnterArticle(fd, serverAddrp);
                    }
                    break;
                case 3: // article
                    if (command.find("Q") == 0u) {
                        nowArticleIndex = -1;
                        nowStage =NPStage::MAIN;
                    }
                    else if (command.find("C") == 0u) {
                        ClientUtility::udpCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("EC") == 0u) {
                        ClientUtility::udpEditCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("DC") == 0u) {
                        ClientUtility::udpDeleteCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("L") == 0u) {
                        ClientUtility::udpLikeArticle(fd, serverAddrp);
                    }
                    else if (command.find("UL") == 0u) {
                        ClientUtility::udpUnlikeArticle(fd, serverAddrp);
                    }
                    else if (command.find("E") == 0u) {
                        ClientUtility::udpEditArticle(fd, serverAddrp);
                    }
                    else if (command.find("D") == 0u) {
                        ClientUtility::udpDeleteArticle(fd, serverAddrp);
                    }
            }
            printMessage(nowStage);
        }
    }
}

void printMessage(const NPStage& stage) {
    switch (static_cast<int>(stage)) {
        case 0: // init
            break;
        case 1: // welcome
            printf("%s~ ", msgOptWELCOME.c_str());
            break;
        case 2: // main
            printf("%s~ ", msgOptMAIN.c_str());
            break;
        case 3: // article
            printf("%s~ ", msgOptARTICLE.c_str());
            break;
    }
    fflush(stdout);
}


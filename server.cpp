#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include "global.h"
#include "nputility.h"
#include "udpmessage.h"
#include "UDPUtil.h"

UDPUtil udp;
std::string lastIP;
std::string lastPort;

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

// store article
class ArticleData {
    public:
        ArticleData() {
            timeStamp = time(NULL);
        }

        virtual ~ArticleData() {

        }

    public:
        // time
        time_t timeStamp;
        // title
        std::string title;
        // article content
        std::string content;
        // author
        std::string author;
        // source ip, port -> string
        std::string source;
        // who says like
        std::vector<std::string> liker;
        // comment
        std::vector<std::string> comment;
        // viewers setting
        NPArticlePermission permission;
        // viewer list(only when permission == SPEC
        std::vector<std::string> viewer;
};

class Articles {
    public:
        Articles() {
            index = 0;
        }

        virtual ~Articles() {

        }

        void incIndex() {
            index++;
        }

        int getIndex() const {
            return index;
        }

        void newArticle(int index) {
            articles.insert(std::make_pair(index, ArticleData()));
        }

        ArticleData& getArticle(int index) {
            return articles[index];
        }

    private:
        int index;
        // map for article index --> article content
        std::map<int, ArticleData> articles;
};

// map for Account --> UserData
std::map<std::string, UserData> serverData;
// articles
Articles articles;

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
                serverData[account].name = account;
                serverData[account].birthday = "???";
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
            printf("Account %s logout at %s", account, asctime(localtime(&serverData[account].lastLogin)));
            udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
        }

        static void udpDeleteAccount(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            sscanf(msg.c_str(), "%*s%s", account);
            serverData.erase(account);
            std::string toSend = "Delete Successfully!\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
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

        static void udpSetProfile(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            char name[MAXN];
            char birthday[MAXN];
            sscanf(msg.c_str(), "%*s%s%s%s", account, name, birthday);
            serverData[account].name = name;
            serverData[account].birthday = birthday;
            std::string toSend = "Profile Setting Success!\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpAddArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: ADDARTICLE 0 account viewerType [viewers]
            // server return a new article index
            // format: ADDARTICLE 1 index title
            // server return article index
            // format: ADDARTICLE 2 index content
            // server return SUCCESS MSG
            char account[MAXN];
            int config;
            sscanf(msg.c_str(), "%*s%d", &config);
            if (config == 0) {
                std::istringstream iss(msg.c_str() + 12);
                int viewType;
                iss >> account >> viewType;
                int index = articles.getIndex();
                articles.incIndex();
                articles.newArticle(index);
                articles.getArticle(index).author = account;
                articles.getArticle(index).timeStamp = time(NULL);
                articles.getArticle(index).source = lastIP + ":" + lastPort;
                if (viewType == 0) {
                    articles.getArticle(index).permission = NPArticlePermission::PUBLIC;
                }
                else if (viewType == 1) {
                    articles.getArticle(index).permission = NPArticlePermission::AUTHOR;
                }
                else if (viewType == 2) {
                    articles.getArticle(index).permission = NPArticlePermission::FRIENDS;
                }
                else if (viewType == 3) {
                    articles.getArticle(index).permission = NPArticlePermission::SPEC;
                    std::string viewer;
                    while (iss >> viewer) {
                        articles.getArticle(index).viewer.push_back(viewer);
                    }
                }
                char buffer[MAXN];
                snprintf(buffer, MAXN, "%d", index);
                udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
            }
            else if (config == 1) {
                char indexString[MAXN];
                int index;
                unsigned pos;
                sscanf(msg.c_str() + 12, "%d", &index);
                snprintf(indexString, MAXN, "%d", index);
                pos = msg.find(indexString, 12);
                pos += (strlen(indexString) + 1);
                std::string title;
                if (pos < msg.length()) {
                    title = msg.substr(pos);
                }
                else {
                    title = "";
                }
                printf("title = %s\n", title.c_str());
                articles.getArticle(index).title = title;
                char buffer[MAXN];
                snprintf(buffer, MAXN, "%d", index);
                udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
            }
            else if (config == 2) {
                char indexString[MAXN];
                int index;
                unsigned pos;
                sscanf(msg.c_str() + 12, "%d", &index);
                snprintf(indexString, MAXN, "%d", index);
                pos = msg.find(indexString, 12);
                pos += (strlen(indexString) + 1);
                std::string content;
                if (pos < msg.length()) {
                    content = msg.substr(pos);
                }
                else {
                    content = "";
                }
                articles.getArticle(index).content = content;
                std::string toSend = "Article Added Successfully!\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
            }
        }

        static void udpEnterArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: ENTERARTICLE index
            int index;
            sscanf(msg.c_str(), "%*s%d", &index);
            std::string timeString = asctime(localtime(&articles.getArticle(index).timeStamp));
            std::string toSend = "\n";
            toSend += "Title: " + articles.getArticle(index).title + "\n";
            toSend += "   By: " + articles.getArticle(index).author + "\n";
            toSend += " From: " + articles.getArticle(index).source + "\n";
            toSend += "   At: " + timeString + "\n";
            toSend += articles.getArticle(index).content + "\n";
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
            if (std::string(buffer) == "Q" ||
                std::string(buffer) == "q" ||
                std::string(buffer) == "QUIT" ||
                std::string(buffer) == "quit") {
                return;
            }
        }
        if (FD_ISSET(fd, &fdset)) {
            // TODO: complete it
            char buffer[MAXN];
            memset(buffer, 0, sizeof(buffer));
            udp.udpRecv(fd, clientAddrp, buffer, MAXN);
            lastIP = inet_ntoa(clientAddr.sin_addr);
            lastPort = std::to_string(static_cast<int>(ntohs(clientAddr.sin_port)));
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
            else if (msg.find(msgDELETEACCOUNT) == 0u) {
                ServerUtility::udpDeleteAccount(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSHOWPROFILE) == 0u) {
                ServerUtility::udpShowProfile(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSETPROFILE) == 0u) {
                ServerUtility::udpSetProfile(fd, clientAddrp, msg);
            }
            else if (msg.find(msgADDARTICLE) == 0u) {
                ServerUtility::udpAddArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgENTERARTICLE) == 0u) {
                ServerUtility::udpEnterArticle(fd, clientAddrp, msg);
            }
        }
    }
}


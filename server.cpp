#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <map>
#include <deque>
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
            friendRequest.clear();
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
        std::map<std::string, bool> friendRequest;
        std::map<std::string, bool> friends;
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
        std::map<std::string, bool> liker;
        // comment
        std::vector<std::string> comment;
        // viewer setting
        NPArticlePermission permission;
        // viewer list(only when permission == SPEC
        std::map<std::string, bool> viewer;
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

        void removeArticle(int index) {
            articles.erase(index);
        }

        ArticleData& getArticle(int index) {
            return articles[index];
        }

        const std::map<int, ArticleData>& getAllArticles() const {
            return articles;
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
                snprintf(buffer, MAXN, "Login Success!\n\nWelcome %s\nLast Login %s\n",
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
            snprintf(buffer, MAXN, "Logout Success!\n\n");
            serverData[account].isOnline = false;
            printf("Account %s logout at %s", account, asctime(localtime(&serverData[account].lastLogin)));
            udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
        }

        static void udpDeleteAccount(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            sscanf(msg.c_str(), "%*s%s", account);
            serverData.erase(account);
            for (auto user : serverData) {
                user.second.friends.erase(account);
                user.second.friendRequest.erase(account);
            }
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
            // format: ADDARTICLE 0 account viewerType [viewer]
            // server return a new article index
            // format: ADDARTICLE 1 index title
            // server return article index
            // format: ADDARTICLE 2 index content
            // server return SUCCESS MSG
            char account[MAXN];
            int config;
            sscanf(msg.c_str(), "%*s%d", &config);
            if (config == 0) {
                std::istringstream iss(msg.c_str() + msgADDARTICLE.length() + 3);
                int viewType;
                iss >> account >> viewType;
                int index = articles.getIndex();
                articles.incIndex();
                articles.newArticle(index);
                articles.getArticle(index).author = account;
                articles.getArticle(index).timeStamp = time(NULL);
                articles.getArticle(index).source = lastIP + ":" + lastPort;
                if (viewType == 1) {
                    articles.getArticle(index).permission = NPArticlePermission::PUBLIC;
                }
                else if (viewType == 2) {
                    articles.getArticle(index).permission = NPArticlePermission::AUTHOR;
                }
                else if (viewType == 3) {
                    articles.getArticle(index).permission = NPArticlePermission::FRIENDS;
                }
                else if (viewType == 4) {
                    articles.getArticle(index).permission = NPArticlePermission::SPEC;
                    std::string viewer;
                    while (iss >> viewer) {
                        articles.getArticle(index).viewer.insert(std::make_pair(viewer, true));
                    }
                    articles.getArticle(index).viewer.insert(std::make_pair(account, true));
                }
                char buffer[MAXN];
                snprintf(buffer, MAXN, "%d", index);
                udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
            }
            else if (config == 1) {
                int index;
                unsigned pos;
                sscanf(msg.c_str() + msgADDARTICLE.length() + 3, "%d", &index);
                pos = msg.find(std::to_string(index), msgADDARTICLE.length() + 3);
                pos += (std::to_string(index).length() + 1);
                std::string title;
                if (pos < msg.length()) {
                    title = msg.substr(pos);
                }
                else {
                    title = "";
                }
                articles.getArticle(index).title = title;
                char buffer[MAXN];
                snprintf(buffer, MAXN, "%d", index);
                udp.udpSend(fd, clientAddrp, buffer, strlen(buffer));
            }
            else if (config == 2) {
                int index;
                unsigned pos;
                sscanf(msg.c_str() + msgADDARTICLE.length() + 3, "%d", &index);
                pos = msg.find(std::to_string(index), msgADDARTICLE.length() + 3);
                pos += (std::to_string(index).length() + 1);
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

        static void udpEditArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: EDITARTICLE 0 index viewType [viewer]
            // server return result
            // format: EDITARTICLE 1 index title
            // server return result
            // format: EDITARTICLE 2 index content
            // server return result
            int config, index, offset;
            sscanf(msg.c_str(), "%*s%d%d", &config, &index);
            offset = msgEDITARTICLE.length() + 3;
            offset += (std::to_string(index).length() + 1);
            if (config == 0) {
                std::istringstream iss(msg.c_str() + offset);
                int viewType;
                iss >> viewType;
                articles.getArticle(index).viewer.clear();
                if (viewType == 1) {
                    articles.getArticle(index).permission = NPArticlePermission::PUBLIC;
                }
                else if (viewType == 2) {
                    articles.getArticle(index).permission = NPArticlePermission::AUTHOR;
                }
                else if (viewType == 3) {
                    articles.getArticle(index).permission = NPArticlePermission::FRIENDS;
                }
                else {
                    std::string account;
                    articles.getArticle(index).permission = NPArticlePermission::SPEC;
                    while (iss >> account) {
                        articles.getArticle(index).viewer.insert(std::make_pair(account, true));
                    }
                }
            }
            else if (config == 1) {
                std::string title = msg.substr(offset);
                articles.getArticle(index).title = title;
            }
            else if (config == 2) {
                std::string content = msg.substr(offset);
                articles.getArticle(index).content = content;
            }
            articles.getArticle(index).timeStamp = time(NULL);
            std::string toSend = "Edit Success!\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpDeleteArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: DELETEARTICLE account index
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            std::string toSend;
            if (articles.getArticle(index).author != account ||
                articles.getAllArticles().count(index) < 1) {
                toSend = msgPERMISSIONDENIED;
            }
            else {
                articles.removeArticle(index);
                toSend = msgSUCCESS;
            }
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpShowArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: SHOWARTICLE account
            // 24 time (maybe use 27 for width)
            char account[MAXN];
            char buffer[MAXN];
            sscanf(msg.c_str(), "%*s%s", account);
            std::string toSend = "Index   Time                       Title                          Author\n";
            for (const auto& item : articles.getAllArticles()) {
                if (!canViewArticle(account, item.second)) {
                    continue;
                }
                snprintf(buffer, MAXN, "%5d", item.first);
                toSend += std::string(buffer) + "   ";
                snprintf(buffer, MAXN, "%-26s", asctime(localtime(&item.second.timeStamp)));
                for (char* ptr = buffer; *ptr; ++ptr) {
                    if (*ptr == '\n') {
                        *ptr = ' ';
                    }
                }
                toSend += std::string(buffer) + " ";
                snprintf(buffer, MAXN, "%-30s", item.second.title.c_str());
                toSend += std::string(buffer) + " ";
                snprintf(buffer, MAXN, "%s", item.second.author.c_str());
                toSend += std::string(buffer) + "\n";
            }
            toSend += "\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpEnterArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: ENTERARTICLE account index
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (index >= articles.getIndex() ||
                index < 0 ||
                articles.getAllArticles().count(index) < 1 ||
                !canViewArticle(account, articles.getArticle(index))) {
                std::string toS = msgPERMISSIONDENIED;
                udp.udpSend(fd, clientAddrp, toS.c_str(), toS.length());
                return;
            }
            std::string timeString = asctime(localtime(&articles.getArticle(index).timeStamp));
            std::string toSend = "";
            toSend += "Title: " + articles.getArticle(index).title + "\n";
            toSend += "   By: " + articles.getArticle(index).author + "\n";
            toSend += " From: " + articles.getArticle(index).source + "\n";
            toSend += "   At: " + timeString + "\n";
            toSend += articles.getArticle(index).content + "\n\n";
            toSend += "Like:\n";
            for (const auto& who : articles.getArticle(index).liker) {
                toSend += who.first + " ";
            }
            toSend += "\n\n";
            toSend += "Comment:\n";
            for (const auto& comment : articles.getArticle(index).comment) {
                toSend += comment + "\n";
            }
            toSend += "\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpLikeArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: LIKEARTICLE account index
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (articles.getAllArticles().count(index) < 1) {
                std::string toS = msgARTICLENOTFOUND;
                udp.udpSend(fd, clientAddrp, toS.c_str(), toS.length());
                return;
            }
            bool notInLiker = (articles.getArticle(index).liker.count(account) < 1);
            if (notInLiker) {
                articles.getArticle(index).liker.insert(std::make_pair(account, true));
            }
            std::string toSend = "Like Successfully!\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpUnlikeArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: UNLIKEARTICLE account index
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (articles.getAllArticles().count(index) < 1) {
                std::string toS = msgARTICLENOTFOUND;
                udp.udpSend(fd, clientAddrp, msg.c_str(), msg.length());
                return;
            }
            articles.getArticle(index).liker.erase(account);
            std::string toSend = "Unlike Successfully\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpCommentArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: COMMENTARTICLE account index message
            char account[MAXN];
            int index;
            unsigned offset = 0;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (articles.getAllArticles().count(index) < 1) {
                std::string toS = msgARTICLENOTFOUND;
                udp.udpSend(fd, clientAddrp, msg.c_str(), msg.length());
                return;
            }
            offset += msgCOMMENTARTICLE.length() + 1;
            offset += (strlen(account) + 1);
            offset += (std::to_string(index).length() + 1);
            std::string comment;
            if (offset < msg.length()) {
                comment = msg.substr(offset);
            }
            else {
                comment = "";
            }
            std::string toAdd = std::string(account) + ": " + comment;
            articles.getArticle(index).comment.push_back(toAdd);
            std::string toSend = "Comment Successful!\n";
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpEditCommentArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: EDITCOMMENTARTICLE account index message
            char account[MAXN];
            int index;
            unsigned offset = 0;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (articles.getAllArticles().count(index) < 1) {
                std::string toS = msgARTICLENOTFOUND;
                udp.udpSend(fd, clientAddrp, msg.c_str(), msg.length());
                return;
            }
            offset += msgEDITCOMMENTARTICLE.length() + 1;
            offset += (strlen(account) + 1);
            offset += (std::to_string(index).length() + 1);
            std::string comment;
            if (offset < msg.length()) {
                comment = msg.substr(offset);
            }
            else {
                comment = "";
            }
            std::string toAdd = std::string(account) + ": " + comment;
            for (auto it = articles.getArticle(index).comment.rbegin();
                 it != articles.getArticle(index).comment.rend();
                 ++it) {
                if (it->find(std::string(account) + ": ") == 0u) {
                    *it = toAdd;
                    std::string toSend = "Comment Successful!\n";
                    udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                    return;
                }
            }
            std::string toError = "You didn\'t comment before!\n";
            udp.udpSend(fd, clientAddrp, toError.c_str(), toError.length());
        }

        static void udpDeleteCommentArticle(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            if (articles.getAllArticles().count(index) < 1) {
                std::string toS = msgARTICLENOTFOUND;
                udp.udpSend(fd, clientAddrp, msg.c_str(), msg.length());
                return;
            }
            int pos = -1;
            for (int i = 0; i < static_cast<int>(articles.getArticle(index).comment.size()); ++i) {
                const std::string& comment = articles.getArticle(index).comment.at(i);
                if (comment.find(std::string(account) + ": ") == 0u) {
                    pos = i;
                }
            }
            if (pos != -1) {
                articles.getArticle(index).comment.erase(
                        articles.getArticle(index).comment.begin() + pos
                );
                std::string toSend = "Delete Comment Successfully!\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
            }
            else {
                std::string toError = "You didn\'t comment before!\n";
                udp.udpSend(fd, clientAddrp, toError.c_str(), toError.length());
            }
        }

        static void udpShowFriends(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            sscanf(msg.c_str(), "%*s%s", account);
            std::string result = "Friends: \n";
            for (const auto& who : serverData[account].friends) {
                if (serverData.count(who.first) < 1) {
                    continue;
                }
                result += std::string("    ") + who.first + "  ";
                if (serverData[who.first].isOnline) {
                    result += "[Online]\n";
                }
                else {
                    std::string lastOnline = asctime(localtime(&serverData[who.first].lastLogin));
                    result += "[Offline]  Last Login: " + lastOnline;
                }
            }
            result += "Requests: \n";
            for (const auto& who : serverData[account].friendRequest) {
                if (serverData.count(who.first) < 1) {
                    continue;
                }
                result += std::string("    ") + who.first + "\n";
            }
            result += "\n";
            udp.udpSend(fd, clientAddrp, result.c_str(), result.length());
        }

        static void udpSearchUser(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format: SEARCHUSER type keyword
            int type;
            unsigned pos = 0;
            sscanf(msg.c_str(), "%*s%d", &type);
            pos += msgSEARCHUSER.length() + 3;
            std::string keyword;
            if (pos < msg.length()) {
                keyword = msg.substr(pos);
            }
            else {
                keyword = "";
            }
            std::string toSend = "Search Result(Account / Name):\n";
            if (type == 1) {
                for (const auto& who : serverData) {
                    if (who.first == keyword) {
                        toSend += who.first + " / " + who.second.name + "\n";
                    }
                }
            }
            else {
                for (const auto& who : serverData) {
                    if (who.second.name == keyword) {
                        toSend += who.first + " / " + who.second.name + "\n";
                    }
                }
            }
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

        static void udpSendFriendRequest(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format SENDFRIENDREQUEST src dst
            char source[MAXN];
            char target[MAXN];
            sscanf(msg.c_str(), "%*s%s%s", source, target);
            if (serverData.count(target) < 1) {
                std::string toSend = msgUSERNOTFOUND;
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            if (serverData[target].friends.count(source) == 1) {
                std::string toSend = "Already be friends!\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            if (serverData[target].friendRequest.count(source) == 1) {
                std::string toSend = "Friend Request has already sent!\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            serverData[target].friendRequest.insert(std::make_pair(source, true));
            std::string result = "Friend Request Sent!\n";
            udp.udpSend(fd, clientAddrp, result.c_str(), result.length());
        }

        static void udpAcceptFrientRequest(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format ACCEPTFRIENDREQUEST src dst
            char source[MAXN];
            char target[MAXN];
            sscanf(msg.c_str(), "%*s%s%s", source, target);
            if (serverData.count(target) < 1) {
                std::string toSend = msgUSERNOTFOUND;
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            if (serverData[source].friendRequest.count(target) == 0) {
                std::string toSend = std::string(target) + " doesn\'t send you a friend request\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            serverData[source].friendRequest.erase(target);
            serverData[source].friends.insert(std::make_pair(target, true));
            serverData[target].friends.insert(std::make_pair(source, true));
            std::string result = "Accepted Successfully!\n";
            udp.udpSend(fd, clientAddrp, result.c_str(), result.length());
        }

        static void udpRejectFriendRequest(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            // format REJECTFRIENDREQUEST src dst
            char source[MAXN];
            char target[MAXN];
            sscanf(msg.c_str(), "%*s%s%s", source, target);
            if (serverData.count(target) < 1) {
                std::string toSend = msgUSERNOTFOUND;
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            if (serverData[source].friendRequest.count(target) == 0) {
                std::string toSend = std::string(target) + " doesn\'t send you a friend request\n";
                udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
                return;
            }
            serverData[source].friendRequest.erase(target);
            std::string result = "Rejected Successfully!\n";
            udp.udpSend(fd, clientAddrp, result.c_str(), result.length());
        }

        static void udpCheckArticlePermission(const int& fd, sockaddr*& clientAddrp, const std::string& msg) {
            char account[MAXN];
            int index;
            sscanf(msg.c_str(), "%*s%s%d", account, &index);
            std::string toSend;
            if (index < 0 || index >= articles.getIndex()) {
                toSend = msgPERMISSIONDENIED;
            }
            else if (articles.getArticle(index).author != account) {
                toSend = msgPERMISSIONDENIED;
            }
            else {
                toSend = msgSUCCESS;
            }
            udp.udpSend(fd, clientAddrp, toSend.c_str(), toSend.length());
        }

    private:
        static bool canViewArticle(const std::string& account, const ArticleData& article) {
            if (article.permission == NPArticlePermission::PUBLIC) {
                return true;
            }
            else if (article.permission == NPArticlePermission::AUTHOR) {
                return account == article.author;
            }
            else if (article.permission == NPArticlePermission::FRIENDS) {
                if (serverData.count(article.author) == 0) {
                    return false;
                }
                if (account == article.author) {
                    return true;
                }
                for (const auto& who : serverData[article.author].friends) {
                    if (account == who.first) {
                        return true;
                    }
                }
                return false;
            }
            else {
                return (article.viewer.count(account) > 0);
            }
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
            else if (msg.find(msgSHOWARTICLE) == 0u) {
                ServerUtility::udpShowArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgADDARTICLE) == 0u) {
                ServerUtility::udpAddArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgEDITARTICLE) == 0u) {
                ServerUtility::udpEditArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgDELETEARTICLE) == 0u) {
                ServerUtility::udpDeleteArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgENTERARTICLE) == 0u) {
                ServerUtility::udpEnterArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgLIKEARTICLE) == 0u) {
                ServerUtility::udpLikeArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgUNLIKEARTICLE) == 0u) {
                ServerUtility::udpUnlikeArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgCOMMENTARTICLE) == 0u) {
                ServerUtility::udpCommentArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgEDITCOMMENTARTICLE) == 0u) {
                ServerUtility::udpEditCommentArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgDELETECOMMENTARTICLE) == 0u) {
                ServerUtility::udpDeleteCommentArticle(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSEARCHUSER) == 0u) {
                ServerUtility::udpSearchUser(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSENDFRIENDREQUEST) == 0u) {
                ServerUtility::udpSendFriendRequest(fd, clientAddrp, msg);
            }
            else if (msg.find(msgSHOWFRIENDS) == 0u) {
                ServerUtility::udpShowFriends(fd, clientAddrp, msg);
            }
            else if (msg.find(msgACCEPTFRIENDREQUEST) == 0u) {
                ServerUtility::udpAcceptFrientRequest(fd, clientAddrp, msg);
            }
            else if (msg.find(msgREJECTFRIENDREQUEST) == 0u) {
                ServerUtility::udpRejectFriendRequest(fd, clientAddrp, msg);
            }
            else if (msg.find(msgCHECKARTICLEPERMISSION) == 0u) {
                ServerUtility::udpCheckArticlePermission(fd, clientAddrp, msg);
            }
        }
    }
}


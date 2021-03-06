#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "npinc.h"
#include "nputility.h"
#include "udpmessage.h"
#include "UDPUtil.h"

NPStage nowStage;
UDPUtil udp;

std::string previousMsg;

std::string nowAccount;
int nowArticleIndex;

void clientFunc(const int& fd, sockaddr_in serverAddr);
void cleanScreen();
void showContent(const std::string& msg);
void showPrevious(std::string errmsg = "");
void printOptions(const NPStage& stage);

class ClientUtility {
public:
    static void udpRegister(const int& fd, sockaddr*& serverAddrp) {
        // format: REGISTER account password
        char account[MAXN];
        char password[MAXN];
        char passwordConfirm[MAXN];
        char buffer[MAXN];
        printf("Account: ");
        if (fgets(account, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(account);
        if (!isValidString(account)) {
            showPrevious("Account cannot contain space or tab character");
            return;
        }
        strcpy(password, getpass("Password: "));
        trimNewLine(password);
        if (!isValidString(password)) {
            showPrevious("Password cannot contain space or tab character");
            return;
        }
        strcpy(passwordConfirm, getpass("Confirm Password: "));
        if (std::string(password) != std::string(passwordConfirm)) {
            showPrevious("Passwords didn\'t match");
            return;
        }
        std::string msg = msgREGISTER + " " + account + " " + password;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (checkServerReply(buffer)) {
            showContent(buffer + msgSUCCESS.length() + 1);
        }
        else {
            showPrevious(buffer + msgFAIL.length() + 1);
        }
    }

    static void udpLogin(const int& fd, sockaddr*& serverAddrp) {
        // format: LOGIN accout password
        char account[MAXN];
        char password[MAXN];
        char buffer[MAXN];
        printf("Account: ");
        if (fgets(account, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(account);
        if (!isValidString(account)) {
            showPrevious("Account cannot contain space or tab character");
            return;
        }
        strcpy(password, getpass("Password: "));
        trimNewLine(password);
        if (!isValidString(password)) {
            showPrevious("Password cannot contain space or tab character");
            return;
        }
        std::string msg = msgLOGIN + " " + account + " " + password;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (checkServerReply(buffer)) {
            showContent(buffer + msgSUCCESS.length() + 1);
            nowStage = NPStage::MAIN;
            nowAccount = account;
        }
        else {
            showPrevious(buffer + msgFAIL.length() + 1);
        }
    }

    static void udpLogout(const int& fd, sockaddr*& serverAddrp) {
        // format: LOGOUT account
        char buffer[MAXN];
        std::string msg = msgLOGOUT + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer + msgSUCCESS.length() + 1);
        if (checkServerReply(buffer)) {
            nowStage = NPStage::WELCOME;
            nowAccount = "";
        }
    }

    static void udpDeleteAccount(const int& fd, sockaddr*& serverAddrp) {
        // format: DELETEACCOUNT account
        char buffer[MAXN];
        printf("ARE YOU SURE?(yes/no): ");
        if (fgets(buffer, MAXN, stdin) == NULL) {
            showPrevious("CANCELED");
            return;
        }
        trimNewLine(buffer);
        if (std::string(buffer) == "yes") {
            std::string msg = msgDELETEACCOUNT + " " + nowAccount;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                showPrevious(msgTIMEOUT);
                return;
            }
            showContent(buffer + msgSUCCESS.length() + 1);
            nowStage = NPStage::WELCOME;
            nowAccount = "";
        }
        else {
            showPrevious("CANCELED");
        }
    }

    static void udpShowProfile(const int& fd, sockaddr*& serverAddrp) {
        // format: SHOWPROFILE account
        char buffer[MAXN];
        std::string msg = msgSHOWPROFILE + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer + msgSUCCESS.length() + 1);
    }

    static void udpSetProfile(const int& fd, sockaddr*& serverAddrp) {
        // format: SETPROFILE account name birthday
        char name[MAXN];
        char birthday[MAXN];
        printf("Name: ");
        if (fgets(name, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(name);
        if (!isValidString(name)) {
            showPrevious("Cannot contain space or tab character");
            return;
        }
        printf("Birthday: ");
        if (fgets(birthday, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(birthday);
        if (!isValidString(birthday)) {
            showPrevious("Cannot contain space or tab character");
            return;
        }
        char buffer[MAXN];
        std::string msg = msgSETPROFILE + " " + nowAccount + " " + name + " " + birthday;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer + msgSUCCESS.length() + 1);
    }

    static void udpAddArticle(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: ADDARTICLE 0 account viewerType [viewers]
        // server return article index
        // format: ADDARTICLE 1 index title
        // server return article index
        // format: ADDARTICLE 2 index content
        // server return SUCCESS MSG
        std::string msg;
        msg = msgADDARTICLE + " 0 " + nowAccount;
        char buffer[MAXN];
        cleanScreen();
        printf("New Article\n");
        printf("Set permission(1:public, 2:author, 3:friend, 4:specific): ");
        while (true) {
            if (fgets(buffer, MAXN, stdin) == NULL) {
                showPrevious();
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
                printf("Please enter accounts(one per line) who can view this article\n");
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
            showPrevious(msgTIMEOUT);
            return;
        }
        int articleIndex;
        sscanf(buffer, "%d", &articleIndex);
        printf("\nArticle Index Number is %d\n\n", articleIndex);
        char title[MAXN];
        printf("Title: ");
        if (fgets(title, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(title);
        msg = msgADDARTICLE + " 1 " + std::to_string(articleIndex) + " " + title;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        msg = msgADDARTICLE + " 2 " + std::to_string(articleIndex) + " ";
        char content[MAXN];
        printf("\nContent(press ^D to finish):\n");
        while (fgets(content, MAXN, stdin) != NULL) {
            msg = msg + content;
        }
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer);
    }

    static void udpEditArticle(const int& fd, sockaddr*& serverAddrp) {
        // format: CHECKARTICLEPERMISSION account index
        // no exception
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
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            return;
        }
        while (true) {
            char command[MAXN];
            printf("%s%s:~ ", msgOptEDITARTICLE.c_str(), nowAccount.c_str());
            if (fgets(command, MAXN, stdin) == NULL) {
                showPrevious();
                break;
            }
            trimNewLine(command);
            for (char* ptr = command; *ptr; ++ptr) {
                *ptr = toupper(*ptr);
            }
            if (std::string(command) == "P") {
                printf("Set permission(1:public, 2:author, 3:friend, 4:specific): ");
                msg = msgEDITARTICLE + " 0 " + std::to_string(index);
                while (true) {
                    if (fgets(buffer, MAXN, stdin) == NULL) {
                        showPrevious();
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
                    showPrevious(msgTIMEOUT);
                    return;
                }
                printf("%s\n", buffer);
            }
            else if (std::string(command) == "T") {
                msg = msgEDITARTICLE + " 1 " + std::to_string(index);
                char title[MAXN];
                printf("Title: ");
                if (fgets(title, MAXN, stdin) == NULL) {
                    showPrevious();
                    return;
                }
                trimNewLine(title);
                msg = msg + " " + title;
                if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                    showPrevious(msgTIMEOUT);
                    return;
                }
                printf("%s\n", buffer);
            }
            else if (std::string(command) == "C") {
                msg = msgEDITARTICLE + " 2 " + std::to_string(index) + " ";
                char content[MAXN];
                printf("Content(Press ^D to finish):\n");
                while (fgets(content, MAXN, stdin) != NULL) {
                    msg += content;
                }
                if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                    showPrevious(msgTIMEOUT);
                    return;
                }
                printf("%s\n", buffer);
            }
            else if (std::string(command) == "Q") {
                break;
            }
            else {
                showPrevious("Command not found");
            }
        }
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(index);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            return;
        }
        showContent(buffer);
    }

    static void udpDeleteArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: PERMISSION DENIED
        // format: DELETEARTICLE account index
        char userConfirm[MAXN];
        printf("ARE YOU SURE?(yes/no): ");
        if (fgets(userConfirm, MAXN, stdin) == NULL) {
            showPrevious("CANCELED");
            return;
        }
        trimNewLine(userConfirm);
        if (std::string(userConfirm) != "yes") {
            showPrevious("CANCELED");
            return;
        }
        std::string msg = msgDELETEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        char buffer[MAXN];
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            return;
        }
        showContent(buffer);
        nowArticleIndex = -1;
        nowStage = NPStage::MAIN;
    }

    static void udpShowArticle(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: SHOWARTICLE account
        std::string msg = msgSHOWARTICLE + " " + nowAccount;
        char buffer[MAXN];
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer);
    }

    static void udpEnterArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: PERMISSION DENIED
        // format: ENTERARTICLE account index
        char buffer[MAXN];
        int index;
        printf("Article Index: ");
        while (true) {
            if (fgets(buffer, MAXN, stdin) == NULL) {
                showPrevious();
                return;
            }
            if (sscanf(buffer, "%d", &index) == 1) {
                break;
            }
        }
        std::string msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(index);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            return;
        }
        showContent(buffer);
        nowArticleIndex = index;
        nowStage = NPStage::ARTICLE;
    }

    static void udpLikeArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: ARTICLE NOT FOUND, PERMISSION DENIED
        // format: LIKEARTICLE account index
        std::string msg;
        char buffer[MAXN];
        msg = msgCHECKARTICLEACCESS + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            return;
        }
        msg = msgLIKEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgARTICLENOTFOUND) {
            showPrevious(buffer);
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            return;
        }
        printf("%s\n", buffer);
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(buffer);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            return;
        }
        showContent(buffer);
    }

    static void udpUnlikeArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: ARTICLE NOT FOUND, PERMISSION DENIED
        // format: UNLIKEARTICLE account index
        std::string msg;
        char buffer[MAXN];
        msg = msgCHECKARTICLEACCESS + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            showPrevious(buffer);
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            return;
        }
        msg = msgUNLIKEARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgARTICLENOTFOUND) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        printf("%s\n", buffer);
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        showContent(buffer);
    }

    static void udpCommentArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: ARTICLE NOT FOUND, PERMISSION DENIED
        // format: COMMENTARTICLE account index message
        std::string msg;
        char buffer[MAXN];
        msg = msgCHECKARTICLEACCESS + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        printf("Comment: ");
        if (fgets(buffer, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(buffer);
        msg = msgCOMMENTARTICLE + " " + nowAccount + " " +
              std::to_string(nowArticleIndex) + " " + buffer;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgARTICLENOTFOUND) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        std::string commentResult = buffer;
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        else {
            showContent(buffer);
        }
        if (commentResult != msgSUCCESS) {
            showPrevious(commentResult);
        }
    }

    static void udpEditCommentArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: ARTICLE NOT FOUND, PERMISSION DENIED
        // format: EDITCOMMENTARTICLE account index message
        std::string msg;
        char buffer[MAXN];
        msg = msgCHECKARTICLEACCESS + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        printf("Comment: ");
        if (fgets(buffer, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(buffer);
        msg = msgEDITCOMMENTARTICLE + " " + nowAccount + " " +
              std::to_string(nowArticleIndex) + " " + buffer;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgARTICLENOTFOUND) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        std::string commentResult = buffer;
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        else {
            showContent(buffer);
        }
        if (commentResult != msgSUCCESS) {
            showPrevious(commentResult);
        }
    }

    static void udpDeleteCommentArticle(const int& fd, sockaddr*& serverAddrp) {
        // exception: ARTICLE NOT FOUND, PERMISSION DENIED
        // format DELETECOMMENTARTICLE account index
        std::string msg;
        char buffer[MAXN];
        msg = msgCHECKARTICLEACCESS + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        msg = msgDELETECOMMENTARTICLE + " " + nowAccount + " " +
              std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgARTICLENOTFOUND) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        std::string commentResult = buffer;
        msg = msgENTERARTICLE + " " + nowAccount + " " + std::to_string(nowArticleIndex);
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgPERMISSIONDENIED) {
            nowArticleIndex = -1;
            nowStage = NPStage::MAIN;
            showPrevious(buffer);
            return;
        }
        else {
            showContent(buffer);
        }
        if (commentResult != msgSUCCESS) {
            showPrevious(commentResult);
        }
    }

    static void udpShowFriends(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: SHOWFRIENDS account
        std::string msg = msgSHOWFRIENDS + " " + nowAccount;
        char buffer[MAXN];
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        nowStage = NPStage::FRIENDS;
        showContent(buffer);
    }

    static void udpSearchUser(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: SEARCHUSER type keyword
        char buffer[MAXN];
        int type;
        printf("Search By [1]Account [2]Name: ");
        while (true) {
            if (fgets(buffer, MAXN, stdin) == NULL) {
                showPrevious();
                return;
            }
            if (sscanf(buffer, "%d", &type) == 1 && type >= 1 && type <= 2) {
                break;
            }
            else {
                printf("Search By [1]Account [2]Name: ");
            }
        }
        printf("Keyword: ");
        if (fgets(buffer, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(buffer);
        std::string msg = msgSEARCHUSER + " " + std::to_string(type) + " " + buffer;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        nowStage = NPStage::SEARCH;
        showContent(buffer);
    }

    static void udpSendFriendRequest(const int& fd, sockaddr*& serverAddrp) {
        // exception: USER NOT FOUND, ALREADY SENT, ALREADY FRIENDS
        // format: SENDFRIENDREQUEST account targetaccount
        char target[MAXN];
        char buffer[MAXN];
        printf("Account to send friend request: ");
        if (fgets(target, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(target);
        if (std::string(target) == nowAccount) {
            showPrevious("Can\'t send friend request to yourself");
            return;
        }
        std::string msg = msgSENDFRIENDREQUEST + " " + nowAccount + " " + target;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer) == msgSUCCESS) {
            showContent(buffer);
        }
        else {
            showPrevious(buffer);
        }
    }

    static void udpAcceptFrientRequest(const int& fd, sockaddr*& serverAddrp) {
        // exception: USER NOT FOUND, NO FRIEND REQUEST
        // format ACCEPTFRIENDREQUEST src dst
        char target[MAXN];
        char buffer[MAXN];
        printf("Account to accept request: ");
        if (fgets(target, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(target);
        std::string msg = msgACCEPTFRIENDREQUEST + " " + nowAccount + " " + target;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        std::string result = buffer;
        msg = msgSHOWFRIENDS + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer);
        if (result != msgSUCCESS) {
            showPrevious(result);
        }
    }

    static void udpRejectFriendRequest(const int& fd, sockaddr*& serverAddrp) {
        // exception: USER NOT FOUND, NO FRIEND REQUEST
        // format REJECTFRIENDREQUEST src dst
        char target[MAXN];
        char buffer[MAXN];
        printf("Account to reject request: ");
        if (fgets(target, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(target);
        std::string msg = msgREJECTFRIENDREQUEST + " " + nowAccount + " " + target;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        std::string result = buffer;
        msg = msgSHOWFRIENDS + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer);
        if (result != msgSUCCESS) {
            showPrevious(result);
        }
    }

    static void udpDeleteFriend(const int& fd, sockaddr*& serverAddrp) {
        // exception: NOT FRIENDS
        // format: DELETEFRIEND account target
        printf("Friend Account to delete: ");
        char target[MAXN];
        char buffer[MAXN];
        if (fgets(target, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        char userConfirm[MAXN];
        printf("ARE YOU SURE?(yes/no): ");
        if (fgets(userConfirm, MAXN, stdin) == NULL) {
            showPrevious("CANCELED");
            return;
        }
        trimNewLine(userConfirm);
        if (std::string(userConfirm) != "yes") {
            showPrevious("CANCELED");
            return;
        }
        trimNewLine(target);
        std::string msg = msgDELETEFRIEND + " " + nowAccount + " " + target;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        std::string result = buffer;
        msg = msgSHOWFRIENDS + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        showContent(buffer);
        if (result != msgSUCCESS) {
            showPrevious(result);
        }
    }

    static void udpGetChatUsers(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: GETCHATUSERS account
        std::string msg = msgGETCHATUSERS + " " + nowAccount;
        char buffer[MAXN];
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        nowStage = NPStage::CHAT;
        showContent(buffer);
    }

    static void udpChatIndividual(const int& fd, sockaddr*& serverAddrp) {
        // format: MESSAGE type source target message
        char target[MAXN];
        std::string targetAcc;
        printf("Account to Chat: ");
        if (fgets(target, MAXN, stdin) == NULL) {
            showPrevious();
            return;
        }
        trimNewLine(target);
        targetAcc = target;
        char buffer[MAXN];
        char input[MAXN];
        std::string msg;
        msg = msgCHECKUSEREXIST + " " + targetAcc;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer).find(msgFAIL) == 0u) {
            showPrevious("User not found!");
            return;
        }
        printf("Type \"/upload <file>\" to Upload File\n");
        printf("Type \"/download <file>\" to Download File\n");
        printf("Press ^D to exit\n");
        msg = msgFLUSHCHAT + " " + msgCHATINDIVIDUAL + " " + nowAccount + " " + targetAcc;
        fd_set fdset;
        FD_ZERO(&fdset);
        while (true) {
            FD_SET(fileno(stdin), &fdset);
            timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 200000;
            int nready = select(fileno(stdin) + 1, &fdset, NULL, NULL, &tv);
            if (nready < 0) {
                if (errno == EINTR) {
                    continue;
                }
                else {
                    fprintf(stderr, "select: %s\n", strerror(errno));
                    break;
                }
            }
            if (FD_ISSET(fileno(stdin), &fdset)) {
                if (fgets(input, MAXN, stdin) == NULL) {
                    break;
                }
                trimNewLine(input);
                if (input[0] == '/') {
                    if (std::string(input).find("/upload") != 0u &&
                        std::string(input).find("/download") != 0u) {
                        continue;
                    }
                    char filename[MAXN];
                    processArgument(filename, input);
                    if (strlen(filename) == 0) {
                        continue;
                    }
                    bool flag = false;
                    if (std::string(input).find("/upload") == 0u) {
                        udpUploadFile(fd, serverAddrp, filename, flag);
                        if (flag) {
                            std::string upr = msgMESSAGE + " " + msgCHATINDIVIDUAL + " " +
                                              nowAccount + " " + targetAcc + " " +
                                              nowAccount + " uploaded " + filename;
                            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, upr.c_str(), upr.length()) < 0) {
                                break;
                            }
                        }
                    }
                    if (std::string(input).find("/download") == 0u) {
                        udpDownloadFile(fd, serverAddrp, filename,  flag);
                        if (flag) {
                            std::string dlr = msgMESSAGE + " " + msgCHATINDIVIDUAL + " " +
                                              nowAccount + " " + targetAcc + " " +
                                              nowAccount + " downloaded " + filename;
                            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, dlr.c_str(), dlr.length()) < 0) {
                                break;
                            }
                        }
                    }
                    continue;
                }
                std::string toSend = msgMESSAGE + " " + msgCHATINDIVIDUAL + " " +
                                     nowAccount + " " + targetAcc + " " + input;
                if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, toSend.c_str(), toSend.length()) < 0) {
                    break;
                }
            }
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                break;
            }
            std::string content = buffer;
            if (content.length() > 0) {
                printf("%s", content.c_str());
            }
        }
        printf("\nExited!\n");
        udpGetChatUsers(fd, serverAddrp);
    }

    static void udpListChatGroup(const int& fd, sockaddr*& serverAddrp) {
        // no exception
        // format: LISTCHATGROUP
        std::string msg = msgLISTCHATGROUP;
        char buffer[MAXN];
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        nowStage = NPStage::CHATGROUP;
        showContent(buffer);
    }

    static void udpChatGroup(const int& fd, sockaddr*& serverAddrp, const std::string& config) {
        char groupname[MAXN];
        char buffer[MAXN];
        char input[MAXN];
        std::string msg;
        while (true) {
            printf("Group Name: ");
            if (fgets(groupname, MAXN, stdin) == NULL) {
                showPrevious();
                return;
            }
            trimNewLine(groupname);
            if (!isValidString(groupname)) {
                printf("Group name cannot contain space or tab character\n");
            }
            else {
                break;
            }
        }
        if (config == "C") {
            msg = msgENTERCHATGROUP + " " + msgNEWGROUP + " " + nowAccount + " " + groupname;
        }
        else {
            msg = msgENTERCHATGROUP + " " + msgEXISTGROUP + " " + nowAccount + " " + groupname;
        }
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(buffer).find(msgFAIL) == 0u) {
            std::string errmsg = buffer + msgFAIL.length() + 1;
            showPrevious(errmsg);
            return;
        }
        printf("%s\n", buffer);
        printf("Type \"/upload <file>\" to Upload File\n");
        printf("Type \"/download <file>\" to Download File\n");
        printf("Press ^D to exit\n");
        msg = msgFLUSHCHAT + " " + msgCHATGROUP + " " + nowAccount + " " + groupname;
        fd_set fdset;
        FD_ZERO(&fdset);
        while (true) {
            FD_SET(fileno(stdin), &fdset);
            timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 200000;
            int nready = select(fileno(stdin) + 1, &fdset, NULL, NULL, &tv);
            if (nready < 0) {
                if (errno == EINTR) {
                    continue;
                }
                else {
                    fprintf(stderr, "select %s\n", strerror(errno));
                    break;
                }
            }
            if (FD_ISSET(fileno(stdin), &fdset)) {
                if (fgets(input, MAXN, stdin) == NULL) {
                    break;
                }
                trimNewLine(input);
                if (input[0] == '/') {
                    if (std::string(input).find("/upload") != 0u &&
                        std::string(input).find("/download") != 0u) {
                        continue;
                    }
                    char filename[MAXN];
                    processArgument(filename, input);
                    if (strlen(filename) == 0) {
                        continue;
                    }
                    bool flag = false;
                    if (std::string(input).find("/upload") == 0u) {
                        udpUploadFile(fd, serverAddrp, filename, flag);
                        if (flag) {
                            std::string upr = msgMESSAGE + " " + msgCHATGROUP + " " +
                                              nowAccount + " " + groupname + " " +
                                              nowAccount + " uploaded " + filename;
                            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, upr.c_str(), upr.length()) < 0) {
                                break;
                            }
                        }
                    }
                    if (std::string(input).find("/download") == 0u) {
                        udpDownloadFile(fd, serverAddrp, filename,  flag);
                        if (flag) {
                            std::string dlr = msgMESSAGE + " " + msgCHATGROUP + " " +
                                              nowAccount + " " + groupname + " " +
                                              nowAccount + " downloaded " + filename;
                            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, dlr.c_str(), dlr.length()) < 0) {
                                break;
                            }
                        }
                    }
                    continue;
                }
                std::string toSend = msgMESSAGE + " " + msgCHATGROUP + " " +
                                     nowAccount + " " + groupname + " " + input;
                if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, toSend.c_str(), toSend.length()) < 0) {
                    break;
                }
            }
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                break;
            }
            std::string content = buffer;
            if (content.length() > 0) {
                printf("%s", content.c_str());
            }
        }
        msg = msgLEAVECHATGROUP + " " + nowAccount;
        if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
            return;
        }
        printf("\nExited!\n");
        udpListChatGroup(fd, serverAddrp);
    }

    static void udpUploadFile(const int& fd, sockaddr*& serverAddrp, const std::string& argu, bool& flag) {
        // file transfer:
        // start: FILENEW filename
        // server return SUCCESS!
        // continue: FILESEQ filename byteInFile n content(content at most 1500)
        // server return n
        // end: FILEEND filename filesize
        // server return check result
        flag = false;
        char filenameCStr[MAXN];
        char buffer[MAXN];
        char recv[MAXN];
        std::string filename, localFilename;
        snprintf(filenameCStr, MAXN, "%s", argu.c_str());
        filename = filenameCStr;
        localFilename = std::string("Client/") + filenameCStr;
        struct stat fileStat;
        if (stat(localFilename.c_str(), &fileStat) < 0) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
            return;
        }
        if (!S_ISREG(fileStat.st_mode)) {
            fprintf(stderr, "%s: Not a regular file\n", filename.c_str());
            return;
        }
        unsigned long fileSize = fileStat.st_size;
        FILE* fp = fopen(localFilename.c_str(), "rb");
        if (!fp) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
            return;
        }
        snprintf(buffer, MAXN, "%s %s", msgFILENEW.c_str(), filename.c_str());
        if (udp.udpTrans(fd, serverAddrp, recv, MAXN, buffer, strlen(buffer)) < 0) {
            fclose(fp);
            return;
        }
        if (std::string(recv) != msgSUCCESS) {
            fprintf(stderr, "%s\n", recv);
            fclose(fp);
            return;
        }
        unsigned long byteSend = 0;
        time_t timeStamp = time(NULL) - 1;
        while (byteSend < fileSize) {
            int n;
            char filecontent[MAXN];
            n = read(fileno(fp), filecontent, 1500);
            if (n < 0) {
                fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
                fclose(fp);
                return;
            }
            memset(buffer, 0, sizeof(buffer));
            unsigned size;
            size = snprintf(buffer, MAXN, "%s %s %lu %d ",
                    msgFILESEQ.c_str(), filename.c_str(), byteSend, n
            );
            memcpy(buffer + size + 1, filecontent, n);
            size += n + 1;
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, buffer, size) < 0) {
                fclose(fp);
                return;
            }
            if (std::string(recv).find(msgSUCCESS) != 0u) {
                std::string errmsg = filename + ": " + recv;
                fprintf(stderr, "%s: %s\n", filename.c_str(), recv);
                fclose(fp);
                return;
            }
            byteSend += n;
            if (difftime(time(NULL), timeStamp) >= 1.0) {
                unsigned long percent = byteSend * 100 / fileSize;
                printf("\rUploading...%lu%% (%lu/%lu)", percent, byteSend, fileSize);
                fflush(stdout);
                timeStamp = time(NULL);
            }
        }
        printf("\rUploading...100%% (%lu/%lu)\n", fileSize, fileSize);
        fclose(fp);
        unsigned long long easyHash = fileHash(localFilename.c_str());
        snprintf(buffer, MAXN, "%s %s %lu %llx",
                 msgFILEEND.c_str(), filename.c_str(), fileSize, easyHash);
        if (udp.udpTrans(fd, serverAddrp, recv, MAXN, buffer, strlen(buffer)) < 0) {
            showPrevious(msgTIMEOUT);
            return;
        }
        if (std::string(recv) == msgSUCCESS) {
            printf("File Uploaded Successfully!\n");
            flag = true;
        }
        else {
            printf("File Upload Failed!\n Filesize or hash value mismatched!\n");
            printf("Please try again later!\n");
        }
    }

    static void udpDownloadFile(const int& fd, sockaddr*& serverAddrp, const std::string& argu, bool& flag) {
        // format: FILEREQ 0 filename
        // server return SUCCESS fileSize(if success)
        //               FAIL error message(if fail)
        // format: FILEREQ 1 filename offset
        // server return SUCCESS n content(if success)
        //               FAIL error message(if failed)
        // format: FILEREQ 2 filename
        // server return SUCCESS hash(if success)
        //               FAIL error message(if failed)
        flag = false;
        char recv[MAXN];
        char filenameCStr[MAXN];
        std::string filename;
        std::string localFilename;
        std::string msg;
        snprintf(filenameCStr, MAXN, "%s", argu.c_str());
        filename = filenameCStr;
        localFilename = std::string("Client/") + filename;
        msg = msgFILEREQ + " 0 " + filename;
        if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
            return;
        }
        if (std::string(recv).find(msgFAIL) == 0u) {
            fprintf(stderr, "%s\n", recv + msgFAIL.length() + 1);
            return;
        }
        unsigned long fileSize;
        sscanf(recv, "%*s%lu", &fileSize);
        FILE* fp = fopen(localFilename.c_str(), "wb");
        if (!fp) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
            return;
        }
        unsigned long byteRecv = 0;
        time_t timeStamp = time(NULL) - 1;
        while (byteRecv < fileSize) {
            msg = msgFILEREQ + " 1 " + filename + " " + std::to_string(byteRecv);
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
                fclose(fp);
                return;
            }
            if (std::string(recv).find(msgFAIL) == 0u) {
                fprintf(stderr, "%s\n", recv + msgFAIL.length() + 1);
                fclose(fp);
                return;
            }
            int n;
            int offset;
            sscanf(recv, "%*s%d", &n);
            offset = msgSUCCESS.length() + 1 + std::to_string(n).length() + 1 + 1;
            if (write(fileno(fp), recv + offset, n) < 0) {
                fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
                fclose(fp);
                return;
            }
            byteRecv += n;
            if (difftime(time(NULL), timeStamp) >= 1.0) {
                unsigned long percent = byteRecv * 100 / fileSize;
                printf("\rDownloading...%lu%% (%lu/%lu)", percent, byteRecv, fileSize);
                fflush(stdout);
                timeStamp = time(NULL);
            }
        }
        printf("\rDownloading...100%% (%lu/%lu)\n", fileSize, fileSize);
        fclose(fp);
        msg = msgFILEREQ + " 2 " + filename;
        if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
            return;
        }
        if (std::string(msg).find(msgFAIL) == 0u) {
            showPrevious(msg.c_str() + msgFAIL.length() + 1);
            fprintf(stderr, "%s\n", msg.c_str() + msgFAIL.length() + 1);
            return;
        }
        struct stat fileStat;
        if (stat(localFilename.c_str(), &fileStat) < 0) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
            return;
        }
        unsigned long long hash;
        sscanf(recv, "%*s%llx", &hash);
        unsigned long long easyHash = fileHash(localFilename.c_str());
        if (easyHash != hash || static_cast<unsigned long>(fileStat.st_size) != fileSize) {
            fprintf(stderr, "Error! FileSize or Hash value is not matched!\n\n");
            return;
        }
        printf("File Downloaded Successfully!\n");
        flag = true;
    }

private:
    static bool checkServerReply(const std::string& str) {
        if (str.find(msgSUCCESS) == 0u) {
            return true;
        }
        else {
            return false;
        }
    }

    static bool isValidString(const std::string& str) {
        for (char c : str) {
            if (c == ' ' || c == '\t') {
                return false;
            }
        }
        for (char c : str) {
            if (c == '\n') {
                return false;
            }
        }
        return true;
    }

    static void processArgument(char* dst, const char* src) {
        std::string tmp(src);
        unsigned offset;
        if (tmp.find("/upload") == 0u) {
            offset = strlen("/upload") + 1;
        }
        else if (tmp.find("/download") == 0u) {
            offset = strlen("/download") + 1;
        }
        else {
            dst[0] = '\0';
            return;
        }
        while (offset < tmp.length() && tmp[offset] == ' ') {
            ++offset;
        }
        if (offset < tmp.length()) {
            strcpy(dst, tmp.substr(offset).c_str());
        }
        else {
            dst[0] = '\0';
        }
    }
};

int main(int argc, char const** argv) {
    // check argments
    if (argc != 3) {
        fprintf(stderr, "usage %s <server address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // check port
    int port;
    if (sscanf(argv[2], "%d", &port) != 1 || !(port >= 0 && port <= 65535)) {
        fprintf(stderr, "%s is not a valid port number\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    nowAccount = "";
    mkdir("Client", 0777);
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
    int maxfdp1 = std::max(fd, fileno(stdin)) + 1;
    FD_ZERO(&fdset);
    // server sockaddr*
    sockaddr* serverAddrp = reinterpret_cast<sockaddr*>(&serverAddr);
    std::string connectMsg = msgNEWCONNECTION;
    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, connectMsg.c_str(), connectMsg.length()) < 0) {
        return;
    }
    showContent(buffer);
    nowStage = NPStage::WELCOME;
    // loop to select
    bool refreshFlag = true;
    for ( ; ; ) {
        if (refreshFlag) {
            printOptions(nowStage);
            refreshFlag = false;
        }
        // set socket fd
        FD_SET(fd, &fdset);
        FD_SET(fileno(stdin), &fdset);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000;
        int nready = select(maxfdp1, &fdset, NULL, NULL, &tv);
        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                fprintf(stderr, "select: %s\n", strerror(errno));
            }
        }
        if (FD_ISSET(fd, &fdset)) {
            char recvLine[MAXN * 2];
            socklen_t servLen = sizeof(*serverAddrp);
            recvfrom(fd, recvLine, MAXN * 2, 0, serverAddrp, &servLen);
        }
        if (FD_ISSET(fileno(stdin), &fdset)) {
            refreshFlag = true;
            memset(buffer, 0, sizeof(buffer));
            while (fgets(buffer, MAXN, stdin) == NULL) {
                continue;
            }
            trimNewLine(buffer);
            for (char* ptr = buffer; *ptr; ++ptr) {
                *ptr = toupper(*ptr);
            }
            std::string command = buffer;
            if (command == "LQ") {
                if (nowAccount != "") {
                    ClientUtility::udpLogout(fd, serverAddrp);
                }
                return;
            }
            switch (static_cast<int>(nowStage)) {
                case 0: // init
                    showPrevious("Invalid Command\n");
                    break;
                case 1: // welcome
                    if (command.find("Q") == 0u && command.length() == 1) {
                        return;
                    }
                    else if (command.find("L") == 0u && command.length() == 1) {
                        ClientUtility::udpLogin(fd, serverAddrp);
                    }
                    else if (command.find("R") == 0u && command.length() == 1) {
                        ClientUtility::udpRegister(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 2: // main
                    if (command.find("L") == 0u && command.length() == 1) {
                        ClientUtility::udpLogout(fd, serverAddrp);
                    }
                    else if (command.find("Q") == 0u && command.length() == 1) {
                        ClientUtility::udpLogout(fd, serverAddrp);
                    }
                    else if (command.find("DA") == 0u && command.length() == 2) {
                        ClientUtility::udpDeleteAccount(fd, serverAddrp);
                    }
                    else if (command.find("W") == 0u && command.length() == 1) {
                        time_t timeStamp = time(NULL);
                        std::string timeStr = asctime(localtime(&timeStamp));
                        std::string welcomeMsg = std::string("Welcome! ") + nowAccount + "\n\n" +
                                                 std::string("Current Time: ") + std::string(timeStr) + "\n";
                        showContent(welcomeMsg);
                    }
                    else if (command.find("SP") == 0u && command.length() == 2) {
                        ClientUtility::udpShowProfile(fd, serverAddrp);
                    }
                    else if (command.find("SE") == 0u && command.length() == 2) {
                        ClientUtility::udpSetProfile(fd, serverAddrp);
                    }
                    else if (command.find("SA") == 0u && command.length() == 2) {
                        ClientUtility::udpShowArticle(fd, serverAddrp);
                    }
                    else if (command.find("A") == 0u && command.length() == 1) {
                        ClientUtility::udpAddArticle(fd, serverAddrp);
                    }
                    else if (command.find("E") == 0u && command.length() == 1) {
                        ClientUtility::udpEnterArticle(fd, serverAddrp);
                    }
                    else if (command.find("F") == 0u && command.length() == 1) {
                        ClientUtility::udpShowFriends(fd, serverAddrp);
                    }
                    else if (command.find("S") == 0u && command.length() == 1) {
                        ClientUtility::udpSearchUser(fd, serverAddrp);
                    }
                    else if (command.find("C") == 0u && command.length() == 1) {
                        ClientUtility::udpGetChatUsers(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 3: // article
                    if (command.find("Q") == 0u && command.length() == 1) {
                        nowArticleIndex = -1;
                        nowStage =NPStage::MAIN;
                        time_t timeStamp = time(NULL);
                        std::string timeStr = asctime(localtime(&timeStamp));
                        std::string welcomeMsg = std::string("Welcome! ") + nowAccount + "\n\n" +
                                                 std::string("Current Time: ") + std::string(timeStr) + "\n";
                        showContent(welcomeMsg);
                    }
                    else if (command.find("C") == 0u && command.length() == 1) {
                        ClientUtility::udpCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("EC") == 0u && command.length() == 2) {
                        ClientUtility::udpEditCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("DC") == 0u && command.length() == 2) {
                        ClientUtility::udpDeleteCommentArticle(fd, serverAddrp);
                    }
                    else if (command.find("L") == 0u && command.length() == 1) {
                        ClientUtility::udpLikeArticle(fd, serverAddrp);
                    }
                    else if (command.find("UL") == 0u && command.length() == 2) {
                        ClientUtility::udpUnlikeArticle(fd, serverAddrp);
                    }
                    else if (command.find("E") == 0u && command.length() == 1) {
                        ClientUtility::udpEditArticle(fd, serverAddrp);
                    }
                    else if (command.find("D") == 0u && command.length() == 1) {
                        ClientUtility::udpDeleteArticle(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 4: // search
                    if (command.find("Q") == 0u && command.length() == 1) {
                        nowStage = NPStage::MAIN;
                        time_t timeStamp = time(NULL);
                        std::string timeStr = asctime(localtime(&timeStamp));
                        std::string welcomeMsg = std::string("Welcome! ") + nowAccount + "\n\n" +
                                                 std::string("Current Time: ") + std::string(timeStr) + "\n";
                        showContent(welcomeMsg);
                    }
                    else if (command.find("S") == 0u && command.length() == 1) {
                        ClientUtility::udpSendFriendRequest(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 5: // friends
                    if (command.find("Q") == 0u && command.length() == 1) {
                        nowStage = NPStage::MAIN;
                        time_t timeStamp = time(NULL);
                        std::string timeStr = asctime(localtime(&timeStamp));
                        std::string welcomeMsg = std::string("Welcome! ") + nowAccount + "\n\n" +
                                                 std::string("Current Time: ") + std::string(timeStr) + "\n";
                        showContent(welcomeMsg);
                    }
                    else if (command.find("A") == 0u && command.length() == 1) {
                        ClientUtility::udpAcceptFrientRequest(fd, serverAddrp);
                    }
                    else if (command.find("R") == 0u && command.length() == 1) {
                        ClientUtility::udpRejectFriendRequest(fd, serverAddrp);
                    }
                    else if (command.find("D") == 0u && command.length() == 1) {
                        ClientUtility::udpDeleteFriend(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 6: // chat
                    if (command.find("Q") == 0u && command.length() == 1) {
                        nowStage = NPStage::MAIN;
                        time_t timeStamp = time(NULL);
                        std::string timeStr = asctime(localtime(&timeStamp));
                        std::string welcomeMsg = std::string("Welcome! ") + nowAccount + "\n\n" +
                                                 std::string("Current Time: ") + std::string(timeStr) + "\n";
                        showContent(welcomeMsg);
                    }
                    else if (command.find("T") == 0u && command.length() == 1) {
                        ClientUtility::udpChatIndividual(fd, serverAddrp);
                    }
                    else if (command.find("L") == 0u && command.length() == 1) {
                        ClientUtility::udpListChatGroup(fd, serverAddrp);
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                case 7: // char group
                    if (command.find("Q") == 0u && command.length() == 1) {
                        ClientUtility::udpGetChatUsers(fd, serverAddrp);
                    }
                    else if (command.find("C") == 0u && command.length() == 1) {
                        ClientUtility::udpChatGroup(fd, serverAddrp, "C");
                    }
                    else if (command.find("E") == 0u && command.length() == 1) {
                        ClientUtility::udpChatGroup(fd, serverAddrp, "E");
                    }
                    else {
                        showPrevious("Invalid Command\n");
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void cleanScreen() {
    for (int i = 0; i < 50; ++i) {
        fputc('\n', stdout);
    }
}

void showContent(const std::string& msg) {
    cleanScreen();
    printf("%s\n", msg.c_str());
    previousMsg = msg;
}

void showPrevious(std::string errmsg) {
    cleanScreen();
    printf("%s\n", previousMsg.c_str());
    while (errmsg.length() > 0 && errmsg.front() == '\n') {
        if (errmsg.length() <= 1) {
            errmsg = "";
        }
        else {
            errmsg = errmsg.substr(1);
        }
    }
    while (errmsg.length() > 0 && errmsg.back() == '\n') {
        errmsg.pop_back();
    }
    if (errmsg != "") {
        printf("----------------------------------------------------------------------------\n");
        fprintf(stderr, "%s\n", errmsg.c_str());
    }
}

void printOptions(const NPStage& stage) {
    printf("----------------------------------------------------------------------------\n");
    switch (static_cast<int>(stage)) {
        case 0: // init
            break;
        case 1: // welcome
            printf("%s~ ", msgOptWELCOME.c_str());
            break;
        case 2: // main
            printf("%s%s:~ ", msgOptMAIN.c_str(), nowAccount.c_str());
            break;
        case 3: // article
            printf("%s%s:~ ", msgOptARTICLE.c_str(), nowAccount.c_str());
            break;
        case 4: // search
            printf("%s%s:~ ", msgOptSEARCH.c_str(), nowAccount.c_str());
            break;
        case 5: // friends
            printf("%s%s:~ ", msgOptFRIENDS.c_str(), nowAccount.c_str());
            break;
        case 6: // chat
            printf("%s%s:~ ", msgOptCHAT.c_str(), nowAccount.c_str());
            break;
        case 7: // chat Group
            printf("%s%s:~ ", msgOptCHATGROUP.c_str(), nowAccount.c_str());
            break;
        default:
            break;
    }
    fflush(stdout);
}


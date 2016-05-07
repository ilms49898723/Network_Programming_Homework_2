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
                fprintf(stderr, "Account may not contain space or tab character\n\n");
                return;
            }
            strcpy(password, getpass("Password: "));
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n\n");
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
                fprintf(stderr, "Account may not contain space or tab character\n\n");
                return;
            }
            strcpy(password, getpass("Password: "));
            trimNewLine(password);
            if (!isValidString(password)) {
                fprintf(stderr, "Password may not contain space or tab character\n\n");
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

        static void udpShowFriends(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgSHOWFRIENDS + " " + nowAccount;
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            nowStage = NPStage::FRIENDS;
        }

        static void udpSearchUser(const int& fd, sockaddr*& serverAddrp) {
            char buffer[MAXN];
            int type;
            printf("Search By [1]Account [2]Name: ");
            while (fgets(buffer, MAXN, stdin)) {
                if (sscanf(buffer, "%d", &type) == 1 && type >= 1 && type <= 2) {
                    break;
                }
                else {
                    printf("Search By [1]Account [2]Name: ");
                }
            }
            printf("Keyword: ");
            if (fgets(buffer, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(buffer);
            std::string msg = msgSEARCHUSER + " " + std::to_string(type) + " " + buffer;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            nowStage = NPStage::SEARCH;
        }

        static void udpSendFriendRequest(const int& fd, sockaddr*& serverAddrp) {
            // format: SENDFRIENDREQUEST account targetaccount
            char target[MAXN];
            char buffer[MAXN];
            printf("Account to send friend request: ");
            if (fgets(target, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(target);
            if (std::string(target) == nowAccount) {
                fprintf(stderr, "Can\'t send friend request to yourself\n\n");
                return;
            }
            std::string msg = msgSENDFRIENDREQUEST + " " + nowAccount + " " + target;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpAcceptFrientRequest(const int& fd, sockaddr*& serverAddrp) {
            char target[MAXN];
            char buffer[MAXN];
            printf("Account to accept request: ");
            if (fgets(target, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(target);
            std::string msg = msgACCEPTFRIENDREQUEST + " " + nowAccount + " " + target;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            msg = msgSHOWFRIENDS + " " + nowAccount;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpRejectFriendRequest(const int& fd, sockaddr*& serverAddrp) {
            char target[MAXN];
            char buffer[MAXN];
            printf("Account to reject request: ");
            if (fgets(target, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(target);
            std::string msg = msgREJECTFRIENDREQUEST + " " + nowAccount + " " + target;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            msg = msgSHOWFRIENDS + " " + nowAccount;
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
        }

        static void udpGetChatUsers(const int& fd, sockaddr*& serverAddrp) {
            std::string msg = msgGETCHATUSERS + " " + nowAccount;
            char buffer[MAXN];
            if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            printf("\n%s\n", buffer);
            nowStage = NPStage::CHAT;
        }

        static void udpUploadFile(const int& fd, sockaddr*& serverAddrp) {
            // file transfer:
            // start: FILENEW filename
            // server return SUCCESS!
            // continue: FILESEQ filename byteInFile n content(content at most 1500)
            // server return n
            // end: FILEEND filename filesize
            // server return check result
            char filenameCStr[MAXN];
            char buffer[MAXN];
            char recv[MAXN];
            std::string filename, localFilename;
            printf("Filename to Upload: ");
            if (fgets(filenameCStr, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(filenameCStr);
            filename = filenameCStr;
            localFilename = std::string("Download/") + filenameCStr;
            struct stat fileStat;
            if (stat(localFilename.c_str(), &fileStat) < 0) {
                fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
                return;
            }
            if (!S_ISREG(fileStat.st_mode)) {
                fprintf(stderr, "%s: Not a regular file\n\n", filename.c_str());
                return;
            }
            unsigned long fileSize = fileStat.st_size;
            FILE* fp = fopen(localFilename.c_str(), "rb");
            if (!fp) {
                fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
                return;
            }
            snprintf(buffer, MAXN, "%s %s", msgFILENEW.c_str(), filename.c_str());
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, buffer, strlen(buffer)) < 0) {
                fclose(fp);
                return;
            }
            if (std::string(recv) != msgSUCCESS) {
                fprintf(stderr, "\n%s\n", recv);
                fclose(fp);
                return;
            }
            unsigned long byteSend = 0;
            while (byteSend < fileSize) {
                int n;
                char filecontent[MAXN];
                n = read(fileno(fp), filecontent, 1500);
                if (n < 0) {
                    fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
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
                    fprintf(stderr, "%s: %s\n\n", filename.c_str(), recv);
                    fclose(fp);
                    return;
                }
                byteSend += n;
            }
            fclose(fp);
            unsigned long long easyHash = fileHash(localFilename.c_str());
            snprintf(buffer, MAXN, "%s %s %lu %llx",
                    msgFILEEND.c_str(), filename.c_str(), fileSize, easyHash);
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, buffer, strlen(buffer)) < 0) {
                return;
            }
            printf("\n%s\n", recv);
        }

        static void udpDownloadFile(const int& fd, sockaddr*& serverAddrp) {
            // format: FILEREQ 0 filename
            // server return SUCCESS fileSize(if success)
            //               FAIL error message(if fail)
            // format: FILEREQ 1 filename offset
            // server return SUCCESS n content(if success)
            //               FAIL error message(if failed)
            // format: FILEREQ 2 filename
            // server return SUCCESS hash(if success)
            //               FAIL error message(if failed)
            char recv[MAXN];
            char filenameCStr[MAXN];
            std::string filename;
            std::string localFilename;
            std::string msg;
            printf("Filename to Download: ");
            if (fgets(filenameCStr, MAXN, stdin) == NULL) {
                return;
            }
            trimNewLine(filenameCStr);
            filename = filenameCStr;
            localFilename = std::string("Download/") + filename;
            msg = msgFILEREQ + " 0 " + filename;
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            if (std::string(recv).find(msgFAIL) == 0u) {
                fprintf(stderr, "%s\n\n", recv + msgFAIL.length() + 1);
                return;
            }
            unsigned long fileSize;
            sscanf(recv, "%*s%lu", &fileSize);
            FILE* fp = fopen(localFilename.c_str(), "wb");
            if (!fp) {
                fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
                return;
            }
            unsigned long byteRecv = 0;
            while (byteRecv < fileSize) {
                msg = msgFILEREQ + " 1 " + filename + " " + std::to_string(byteRecv);
                if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
                    fclose(fp);
                    return;
                }
                if (std::string(recv).find(msgFAIL) == 0u) {
                    fprintf(stderr, "%s\n\n", recv + msgFAIL.length() + 1);
                    fclose(fp);
                    return;
                }
                int n;
                int offset;
                sscanf(recv, "%*s%d", &n);
                offset = msgSUCCESS.length() + 1 + std::to_string(n).length() + 1 + 1;
                if (write(fileno(fp), recv + offset, n) < 0) {
                    fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
                    fclose(fp);
                    return;
                }
                byteRecv += n;
            }
            fclose(fp);
            msg = msgFILEREQ + " 2 " + filename;
            if (udp.udpTrans(fd, serverAddrp, recv, MAXN, msg.c_str(), msg.length()) < 0) {
                return;
            }
            if (std::string(msg).find(msgFAIL) == 0u) {
                fprintf(stderr, "%s\n\n", msg.c_str() + msgFAIL.length() + 1);
                return;
            }
            struct stat fileStat;
            if (stat(localFilename.c_str(), &fileStat) < 0) {
                fprintf(stderr, "%s: %s\n\n", filename.c_str(), strerror(errno));
                return;
            }
            unsigned long long hash;
            sscanf(recv, "%*s%llx", &hash);
            unsigned long long easyHash = fileHash(localFilename.c_str());
            if (easyHash != hash || static_cast<unsigned long>(fileStat.st_size) != fileSize) {
                fprintf(stderr, "Error! FileSize or Hash value is not matched!\n\n");
                return;
            }
            else {
                printf("Success!\n\n");
            }
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
    std::string connectMsg = msgNEWCONNECTION;
    if (udp.udpTrans(fd, serverAddrp, buffer, MAXN, connectMsg.c_str(), connectMsg.length()) < 0) {
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
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, MAXN, stdin) == NULL) {
                continue;
            }
            trimNewLine(buffer);
            std::string command = buffer;
            if (command == "LQ") {
                if (nowAccount != "") {
                    ClientUtility::udpLogout(fd, serverAddrp);
                }
                return;
            }
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
                    else if (command.find("F") == 0u) {
                        ClientUtility::udpShowFriends(fd, serverAddrp);
                    }
                    else if (command.find("S") == 0u) {
                        ClientUtility::udpSearchUser(fd, serverAddrp);
                    }
                    else if (command.find("C") == 0u) {
                        ClientUtility::udpGetChatUsers(fd, serverAddrp);
                    }
                    else if (command.find("U") == 0u) {
                        ClientUtility::udpUploadFile(fd, serverAddrp);
                    }
                    else if (command.find("D") == 0u) {
                        ClientUtility::udpDownloadFile(fd, serverAddrp);
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 3: // article
                    if (command.find("Q") == 0u) {
                        nowArticleIndex = -1;
                        nowStage =NPStage::MAIN;
                        printf("\n");
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
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 4: // search
                    if (command.find("Q") == 0u) {
                        nowStage = NPStage::MAIN;
                        printf("\n");
                    }
                    else if (command.find("S") == 0u) {
                        ClientUtility::udpSendFriendRequest(fd, serverAddrp);
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 5: // friends
                    if (command.find("Q") == 0u) {
                        nowStage = NPStage::MAIN;
                        printf("\n");
                    }
                    else if (command.find("A") == 0u) {
                        ClientUtility::udpAcceptFrientRequest(fd, serverAddrp);
                    }
                    else if (command.find("R") == 0u) {
                        ClientUtility::udpRejectFriendRequest(fd, serverAddrp);
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 6: // chat
                    if (command.find("Q") == 0u) {
                        nowStage = NPStage::MAIN;
                        printf("\n");
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                case 7: // char group
                    if (command.find("Q") == 0u) {
                        nowStage = NPStage::CHAT;
                        printf("\n");
                    }
                    else {
                        fprintf(stderr, "Invalid Command\n");
                    }
                    break;
                default:
                    break;
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


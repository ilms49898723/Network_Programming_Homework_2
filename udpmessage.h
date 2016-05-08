#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_
#define NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_

#include <string>

const std::string      msgOptWELCOME = std::string("[L]Login    [R]Register    [Q]Quit\n");
const std::string         msgOptMAIN = std::string("[SP]Show Profile    [SE]Set Profile     [W]Welcome Message\n") +
                                       std::string("[SA]Show Article    [A]Add Article      [E]Enter Article\n") +
                                       std::string("[F]Friends          [C]Chat             [S]Search User\n") +
                                       std::string("[DA]Delete Account  [L/Q]Logout\n");
const std::string      msgOptARTICLE = std::string("[L]Like             [UL]Unlike\n") +
                                       std::string("[C]Comment          [EC]Edit Last Comment  [DC]Delete Last Comment\n") +
                                       std::string("[E]Edit Article     [D]Delete Article      [Q]Quit\n");
const std::string         msgOptCHAT = std::string("[T]Talk to someone  [L]List All Chatting Group  [Q]Quit\n");
const std::string    msgOptCHATGROUP = std::string("[C]Create Group     [E]Enter Group      [Q]Quit\n");
const std::string  msgOptEDITARTICLE = std::string("[P]Change Permission  [T]Change Title  [C]Change content  [Q]Quit\n");
const std::string       msgOptSEARCH = std::string("[S]Send Friend Request   [Q]Quit\n");
const std::string      msgOptFRIENDS = std::string("[A]Accept Request   [R]Reject Request  [D]Delete Friend   [Q]Quit\n");

const std::string           msgNEWCONNECTION = "NEWCONNECTION";
const std::string                msgREGISTER = "REGISTER";
const std::string                   msgLOGIN = "LOGIN";
const std::string                  msgLOGOUT = "LOGOUT";
const std::string           msgDELETEACCOUNT = "DELETEACCOUNT";
const std::string             msgSHOWPROFILE = "SHOWPROFILE";
const std::string              msgSETPROFILE = "SETPROFILE";

const std::string              msgADDARTICLE = "ADDARTICLE";
const std::string             msgEDITARTICLE = "EDITARTICLE";
const std::string           msgDELETEARTICLE = "DELETEARTICLE";
const std::string             msgSHOWARTICLE = "SHOWARTICLE";
const std::string            msgENTERARTICLE = "ENTERARTICLE";
const std::string             msgLIKEARTICLE = "LIKEARTICLE";
const std::string           msgUNLIKEARTICLE = "UNLIKEARTICLE";
const std::string          msgCOMMENTARTICLE = "COMMENTARTICLE";
const std::string      msgEDITCOMMENTARTICLE = "EDITCOMMENTARTICLE";
const std::string    msgDELETECOMMENTARTICLE = "DELETECOMMENTARTICLE";
const std::string      msgCHECKARTICLEACCESS = "CHECKARTICLEACCESS";
const std::string  msgCHECKARTICLEPERMISSION = "CHECKARTICLEPERMISSION";

const std::string          msgCHECKUSEREXIST = "CHECKUSEREXIST";
const std::string              msgSEARCHUSER = "SEARCHUSER";
const std::string       msgSENDFRIENDREQUEST = "SENDFRIENDREQUEST";
const std::string             msgSHOWFRIENDS = "SHOWFRIENDS";
const std::string     msgACCEPTFRIENDREQUEST = "ACCEPTFRIENDREQUEST";
const std::string     msgREJECTFRIENDREQUEST = "REJECTFRIENDREQUEST";
const std::string            msgDELETEFRIEND = "DELETEFRIEND";

const std::string           msgLISTCHATGROUP = "LISTCHATGROUP";
const std::string          msgENTERCHATGROUP = "ENTERCHATGROUP";
const std::string          msgLEAVECHATGROUP = "LEAVECHATGROUP";
const std::string               msgFLUSHCHAT = "FLUSHCHAT";
const std::string                 msgMESSAGE = "MESSAGE";
const std::string          msgCHATINDIVIDUAL = "INDIVIDUAL";
const std::string               msgCHATGROUP = "GROUP";
const std::string                msgNEWGROUP = "NEWGROUP";
const std::string              msgEXISTGROUP = "EXISTGROUP";
const std::string            msgGETCHATUSERS = "GETCHATUSERS";

const std::string                 msgFILENEW = "FILENEW";
const std::string                 msgFILESEQ = "FILESEQ";
const std::string                 msgFILEEND = "FILEEND";
const std::string                 msgFILEREQ = "FILEREQ";
const std::string          msgCHECKFILEEXIST = "CHECKFILEEXIST";

const std::string                 msgSUCCESS = "Success!";
const std::string                    msgFAIL = "Failed!";
const std::string              msgPROCESSING = "SERVERPROCESSING!";
const std::string        msgPERMISSIONDENIED = "Permission Denied!";
const std::string            msgUSERNOTFOUND = "User not found!";
const std::string         msgARTICLENOTFOUND = "This article didn\'t exist anymore!";

const std::string                 msgTIMEOUT = "Timeout! Cannot connect to server!";

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_


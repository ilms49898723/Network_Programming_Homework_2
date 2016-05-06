#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_
#define NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_

#include <string>

const std::string msgOptWELCOME = "[L]Login  [R]Register  [Q]Quit\n";
const std::string msgOptARTICLE = std::string("[L]Like [UL]Unlike\n") +
                                  std::string("[C]Comment [EC]Edit Last Comment [DC]Delete Last Comment\n") +
                                  std::string("[E]Edit Article [D]Delete Article [Q]Quit\n");
const std::string msgOptEDITARTICLE = "[P]Change Permission [T]Change Title [C]Change content [Q]Quit\n";
const std::string msgOptMAIN = std::string("[SP]Show Profile [SE]Set Profile\n") +
                               std::string("[SA]Show Article [A]Add Article [E]Enter Article\n") +
                               std::string("[C]Chat [S]Search User\n") +
                               std::string("[L]Logout [DA]Delete Account\n");

const std::string msgNEWCONNECTION = "NEWCONNECTION";
const std::string msgREGISTER = "REGISTER";
const std::string msgLOGIN = "LOGIN";
const std::string msgLOGOUT = "LOGOUT";
const std::string msgDELETEACCOUNT = "DELETEACCOUNT";
const std::string msgSHOWPROFILE = "SHOWPROFILE";
const std::string msgSETPROFILE = "SETPROFILE";
const std::string msgADDARTICLE = "ADDARTICLE";
const std::string msgEDITARTICLE = "EDITARTICLE";
const std::string msgDELETEARTICLE = "DELETEARTICLE";
const std::string msgSHOWARTICLE = "SHOWARTICLE";
const std::string msgENTERARTICLE = "ENTERARTICLE";
const std::string msgLIKEARTICLE = "LIKEARTICLE";
const std::string msgUNLIKEARTICLE = "UNLIKEARTICLE";
const std::string msgCOMMENTARTICLE = "COMMENTARTICLE";
const std::string msgEDITCOMMENTARTICLE = "EDITCOMMENTARTICLE";
const std::string msgDELETECOMMENTARTICLE = "DELETECOMMENTARTICLE";
const std::string msgCHECKARTICLEPERMISSION = "CHECKARTICLEPERMISSION";

const std::string msgSUCCESS = "Success!\n";
const std::string msgPERMISSIONDENIED = "Permission Denied!\n";
const std::string msgARTICLENOTFOUND = "This article doesn\'t exist anymore!\n";

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_


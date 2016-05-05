#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_
#define NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_

#include <string>

const std::string msgOptWELCOME = "[L]Login  [R]Register\n";
const std::string msgOptARTICLE = "[R]Response  [L]Like  [R]Return";
const std::string msgOptMAIN = std::string("[SP]Show Profile  [SA]Show Article  [A]Add Article\n") +
                               std::string("[E]Enter Article [C]Chat [S]Search [L]Logout\n");

const std::string msgNEWCONNECTION = "NEWCONNECTION";
const std::string msgREGISTER = "REGISTER";
const std::string msgLOGIN = "LOGIN";
const std::string msgLOGOUT = "LOGOUT";
const std::string msgSHOWPROFILE = "SHOWPROFILE";

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_


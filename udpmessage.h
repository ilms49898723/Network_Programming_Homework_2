#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_
#define NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_

#include <string>

const std::string msgOptWELCOME = "[L]Login  [R]Register  [Q]Quit\n";
const std::string msgOptARTICLE = "[R]Response  [L]Like  [R]Return";
const std::string msgOptMAIN = std::string("[SP]Show Profile [SE]Set Profile\n") +
                               std::string("[SA]Show Article [A]Add Article [E]Enter Article\n") +
                               std::string("[C]Chat [S]Search [L]Logout\n");

const std::string msgNEWCONNECTION = "NEWCONNECTION";
const std::string msgREGISTER = "REGISTER";
const std::string msgLOGIN = "LOGIN";
const std::string msgLOGOUT = "LOGOUT";
const std::string msgSHOWPROFILE = "SHOWPROFILE";
const std::string msgSETPROFILE = "SETPROFILE";
const std::string msgSHOWARTICLE = "SHOWARTICLE";
const std::string msgADDARTICLE = "ADDARTICLE";

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_UDPMESSAGE_H_


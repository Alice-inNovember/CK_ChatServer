#ifndef UserUtil_HPP
#define UserUtil_HPP

#include "Data.hpp"
#include "Includes.hpp"

void errExit(std::string msg);

int* getUserFds();
void readSockFd(int fd, char* dest, int size);

void RemoveUser(t_svData& svData, t_userData* userData);

#endif
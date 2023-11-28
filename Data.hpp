#ifndef Data_HPP
#define Data_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define PORT 25000

typedef struct s_svData
{
	int soketFd;
	struct sockaddr_in svAddr;
} t_svData;

#endif
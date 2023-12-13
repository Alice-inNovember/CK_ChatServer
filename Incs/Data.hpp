#ifndef Data_HPP
#define Data_HPP

#include "Includes.hpp"

#define PORT_NBR 25000

//서버 설정
#define MAX_USER 1024
#define EPOLL_SIZE 16	 //한번에 받는 이밴트 수

//전송 데이터 사이즈
#define DS_TEXT 512
#define DS_ARG 16

//구조체
typedef struct epoll_event t_epollEvent;
typedef struct sockaddr_in t_sockAddrIn;
typedef struct sockaddr t_sockAddr;

// user data struct
typedef struct s_userData
{
	int fd;
	int channel;
	char name[DS_ARG + 1];
} t_userData;

typedef struct s_svData
{
	t_sockAddrIn addrIn;
	t_epollEvent listenEv;
	t_epollEvent* userEv;
	int epollFd;
	int listenFd;
} t_svData;

#endif
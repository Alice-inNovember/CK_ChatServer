#ifndef data_HPP
#define data_HPP

#define PORT_NBR 25000

//서버 설정
#define MAX_USER 1024
#define EPOLL_SIZE 16	 //한번에 받는 이밴트 수

//전송 데이터 사이즈
#define DS_TEXT 512
#define DS_NAME 16
#define DS_CHANNEL 16
#define DS_CMD 32

//구조체
typedef struct epoll_event t_epollEvent;
typedef struct sockaddr_in t_sockAddrIn;
typedef struct sockaddr t_sockAddr;

// user data struct
typedef struct s_userData
{
	int fd;
	int channel;
	char name[DS_NAME];
} t_userData;

#endif
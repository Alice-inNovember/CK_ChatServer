#ifndef data_HPP
#define data_HPP

#include "common.hpp"

//색상
#define C_NOMAL "\033[0m"
#define C_ERROR "\033[1;31m"
#define C_STATE "\033[1;32m"
#define C_NOTIY "\033[1;34m"

//포트
#define PORT_NBR 25000

//서버 설정
#define MAX_USER 256		// 최대 접속자 수
#define MAX_USER_Q 8		// 최대 접속 대기 큐
#define EP_INIT_SIZE 16		// epoll_create() 초기 크기
#define EP_EVENT_SIZE 16	// epoll_wait() maxevents

//전송 데이터 사이즈
#define DS_TEXT 512	   // text 사이즈
#define DS_ARGV 16	   // 기타 명령어, 아이디, 체널 사이즈

//구조체 typedef
typedef struct sockaddr_in t_sockAddrIn;
typedef struct sockaddr t_sockAddr;
typedef struct epoll_event t_epollEvent;

typedef struct s_userData
{
	char name[DS_ARGV + 1];
	int ch;
	int fd;
	t_sockAddrIn addr;
} t_userData;

typedef struct s_data
{
	int userState[MAX_USER];
	int userChannel[MAX_USER];
	t_epollEvent epEvent;
	int epFd;
	int svFd;
} t_data;

#endif
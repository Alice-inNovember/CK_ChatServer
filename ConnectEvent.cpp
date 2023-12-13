#include "Incs/Data.hpp"
#include "Incs/Includes.hpp"
#include "Incs/Util.hpp"

t_userData* CreateUser(int clientFd)
{
	char command[DS_ARG];
	char name[DS_ARG];
	char msg[DS_TEXT];
	t_userData* userData;

	try {
		readSockFd(clientFd, command, DS_ARG);
		readSockFd(clientFd, name, DS_ARG);
		readSockFd(clientFd, msg, DS_TEXT);
	}
	// 읽는 도중 에러 발생
	catch (std::string msg) {
		//데이터 할당 X
		throw msg;
	}
	userData = new t_userData;
	userData->fd = clientFd;
	userData->channel = -1;
	strcpy(userData->name, name);
	getUserFds()[clientFd] = 1;
	return userData;
}

void userConnectEvent(t_svData& svData)
{
	t_sockAddrIn clientAddr;
	socklen_t clientLen;
	int clientFd;

	try {
		// accept
		clientLen = sizeof(t_sockAddr);
		clientFd =
			accept(svData.listenFd, (t_sockAddr*)&clientAddr, &clientLen);
		throw std::string("ERROR : accept()");

		//sockopt
		int bEnable = 1;
		setsockopt(clientFd, SOL_SOCKET, SO_KEEPALIVE, &bEnable, sizeof(int));
		setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &bEnable, sizeof(int));
		setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, &bEnable, sizeof(int));

		//user create/init
		t_userData* userData = CreateUser(clientFd);
		svData.userEv[clientFd].data.ptr = userData;
	}
	//에러 발생시 소켓 종료 후 리턴
	catch (std::string msg) {
		close(clientFd);
		std::cerr << "ERROR : " << msg << std::endl;
		std::cerr << "ERROR : " << clientFd << "CreateUser() fail" << std::endl;
		return;
	}

	// 이벤트 설정
	// EPOLL_CTL_ADD를 통해 listen 소켓을 이벤트 풀에 추가.
	svData.userEv[clientFd].events = EPOLLIN;
	epoll_ctl(svData.epollFd, EPOLL_CTL_ADD, clientFd, &svData.listenEv);
}

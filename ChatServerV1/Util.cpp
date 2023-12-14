#include "Incs/Data.hpp"
#include "Incs/Includes.hpp"

// 연결 된 유저 데이터 (FD는 순차적으로 열림 0,1,2 이후 listenFd, 다음으론 연결된 유저)
int* getUserFds()
{
	static int userFds[MAX_USER];
	return userFds;
}

void errExit(std::string msg)
{
	perror(msg.data());
	exit(-1);
}

void RemoveUser(t_svData& svData, t_userData* userData)
{
	epoll_ctl(svData.epollFd, EPOLL_CTL_DEL, userData->fd, svData.userEv);
	close(userData->fd);
	getUserFds()[userData->fd] = -1;
	delete userData;
}

void readSockFd(int fd, char* dest, int size)
{
	int retval;

	memset(dest, 0, size);
	retval = read(fd, dest, size);

	// 읽는 도중 에러 발생
	if (retval <= 0) {
		throw std::string("ERROR  : readSockFd() : read()");
	}
}

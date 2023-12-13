#include "Incs/Data.hpp"
#include "Incs/Includes.hpp"
#include "Incs/UserEvent.hpp"
#include "Incs/Util.hpp"

int main(void)
{
	t_svData svData;

	//listen soket 생성
	{
		socklen_t addrLen;
		int returnVal;

		//socket 생성
		addrLen = sizeof(t_sockAddrIn);
		svData.listenFd = socket(AF_INET, SOCK_STREAM, 0);
		if ((svData.listenFd) == -1)
			errExit("ERROR : listenFd socket()");

		//socket
		svData.addrIn.sin_family = AF_INET;
		svData.addrIn.sin_port = htons(PORT_NBR);
		svData.addrIn.sin_addr.s_addr = htonl(INADDR_ANY);

		int bEnable = 1;
		setsockopt(svData.listenFd, SOL_SOCKET, SO_KEEPALIVE, &bEnable,
				   sizeof(int));
		setsockopt(svData.listenFd, SOL_SOCKET, SO_REUSEADDR, &bEnable,
				   sizeof(int));
		setsockopt(svData.listenFd, IPPROTO_TCP, TCP_NODELAY, &bEnable,
				   sizeof(int));
		//bind
		returnVal = bind(svData.listenFd, (t_sockAddr*)&svData.addrIn, addrLen);
		if (returnVal == -1)
			errExit("ERROR : listenFd bind()");

		//listen
		returnVal = listen(svData.listenFd, 16);
		if (returnVal == -1)
			errExit("ERROR : listenFd bind()");
	}

	//epoll 설정
	{
		int returnVal;

		// userEv 포인터를 초기화한다. EPOLL_SIZE
		svData.userEv = new t_epollEvent[EPOLL_SIZE];

		// epoll 파일 디스크립터를 만든다. size 는 0 이상이면 됨;
		svData.epollFd = epoll_create(64);
		if ((svData.epollFd) == -1)
			errExit("ERROR : epoll_create()");

		// 이벤트 설정
		svData.listenEv.events = EPOLLIN;
		svData.listenEv.data.fd = svData.listenFd;
		// EPOLL_CTL_ADD를 통해 listen 소켓을 이벤트 풀에 추가.
		returnVal = epoll_ctl(svData.epollFd, EPOLL_CTL_ADD, svData.listenFd,
							  &svData.listenEv);
		if (returnVal == -1)
			errExit("ERROR : epoll_ctl()");
		memset(getUserFds(), 0, sizeof(int) * MAX_USER);
	}

	while (1) {
		// 사건 발생 시까지 무한 대기
		// epollFd의 사건 발생 시 events에 fd를 채운다.
		// eventCnt은 listen에 성공한 fd의 수
		int eventCnt =
			epoll_wait(svData.epollFd, svData.userEv, EPOLL_SIZE, -1);
		if (eventCnt == -1)
			errExit("ERROR : epoll_wait()");
		for (int i = 0; i < eventCnt; i++) {
			// 듣기 소켓에서 이벤트가 발생 (새로운 연결)
			if (svData.userEv[i].data.fd == svData.listenFd)
				userConnectEvent(svData);
			// 연결 소켓에서 이벤트가 발생함 (메세지 수령)
			else
				userChatEvent(svData, i);
		}
	}
}

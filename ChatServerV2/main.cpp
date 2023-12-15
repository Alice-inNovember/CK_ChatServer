#include "Incs/common.hpp"
#include "Incs/data.hpp"
#include "Incs/event.hpp"

//서버 fd(Listen) 초기화
static void InitServerSock(t_data* data)
{
	int svFd;
	t_sockAddrIn addr;

	//create
	if ((svFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::string("ERROR  : failed socket()");

	//init
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NBR);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//option
	int bEnable = 1;
	setsockopt(svFd, SOL_SOCKET, SO_KEEPALIVE, &bEnable, sizeof(int));
	setsockopt(svFd, SOL_SOCKET, SO_REUSEADDR, &bEnable, sizeof(int));
	setsockopt(svFd, IPPROTO_TCP, TCP_NODELAY, &bEnable, sizeof(int));

	//bind
	if (bind(svFd, (t_sockAddr*)&addr, sizeof(t_sockAddrIn)) == -1)
		throw std::string("ERROR  : failed bind()");

	//listen
	if (listen(svFd, MAX_USER_Q) == -1)
		throw std::string("ERROR  : failed listen()");

	data->svFd = svFd;
}

//서버 Epoll 초기화
static void InitEpoll(t_data* data)
{
	if ((data->epFd = epoll_create(MAX_USER)) == -1)
		throw std::string("ERROR  : failed epoll_create()");

	data->epEvent.events = EPOLLIN;		   // 이벤트 들어오면 알림
	data->epEvent.data.fd = data->svFd;	   // fd 설정
	//listenfd의 상태변화를 감시 대상에 등록
	if (epoll_ctl(data->epFd, EPOLL_CTL_ADD, data->svFd, &data->epEvent) == -1)
		throw std::string("ERROR  : failed epoll_ctl(), svFd EPOLL_CTL_ADD");
}

//서버 데이터 구조체 초기화
static void InitData(t_data* data)
{
	memset(data, 0, sizeof(t_data));
}

//서버 메인 루프, epoll 대기
static void ServerLoop(t_data* data)
{
	t_epollEvent* epEvents;
	int eventCnt;

	epEvents = new t_epollEvent[EP_EVENT_SIZE];
	while (1) {
		eventCnt = epoll_wait(data->epFd, epEvents, EP_EVENT_SIZE, -1);

		if (eventCnt == -1)
			throw std::string("ERROR  : failed epoll_wait()");

		//반환된 eventCnt 만큼 epEvents를 순회하며 수신한 데이터 처리
		for (int evIdx = 0; evIdx < eventCnt; evIdx++) {
			// 새로 연결된 유저(클라이언트)
			if (epEvents[evIdx].data.fd == data->svFd)
				NewUeserEvent(data);
			//기존 유저(클라이언트)
			else
				UserEvent(data, (t_userData*)epEvents[evIdx].data.ptr);
		}
	}
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	t_data data;

	//Init
	try {
		std::cout << C_STATE << "SERVER : Server Init" << std::endl;

		std::cout << C_STATE << "SERVER : Server Data Init " << std::endl;
		InitData(&data);

		std::cout << C_STATE << "SERVER : Server ServerSock Init " << std::endl;
		InitServerSock(&data);

		std::cout << C_STATE << "SERVER : Server Epoll Init " << std::endl;
		InitEpoll(&data);

		std::cout << C_STATE << "SERVER : Server Start!" << std::endl;
		ServerLoop(&data);
	}
	//Critical error
	catch (std::string msg) {
		std::cerr << C_ERROR << msg << std::endl;
		return (-1);
	}

	std::cout << C_STATE << "SERVER : Server End!" << std::endl;
}
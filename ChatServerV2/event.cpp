#include "Incs/common.hpp"
#include "Incs/data.hpp"

std::string pTime()
{
	std::time_t now = std::time(0);
	std::tm* now_tm = std::gmtime(&now);
	char buf[42];
	std::strftime(buf, 42, "%Y/%m/%d %X", now_tm);
	return buf;
}

static void RemoveUser(t_data* data, t_userData* userData)
{
	data->userState[userData->fd] = 0;
	data->userChannel[userData->fd] = 0;
	epoll_ctl(data->epFd, EPOLL_CTL_DEL, userData->fd, NULL);
	close(userData->fd);
	delete userData;
}

static void readSockFd(int fd, char* dest, int size)
{
	int retval;

	memset(dest, 0, size);
	retval = read(fd, dest, size);

	// 읽는 도중 에러 발생
	if (retval <= 0) {
		throw std::string("ERROR  : readSockFd() : read()");
	}
}

static char* CombinedMessage(char* arg1, char* arg2, const char* msg)
{
	char* dest = new char[DS_ARGV + DS_ARGV + DS_TEXT];

	for (int i = 0; i < DS_ARGV; i++) {
		dest[i] = arg1[i];
		dest[i + DS_ARGV] = arg2[i];
	}
	for (int i = 0; i < DS_TEXT; i++) {
		dest[(DS_ARGV * 2) + i] = msg[i];
	}
	return dest;
}

// 메시지를 하나의 유저(client)에게 전송
void SendNotification(int fd, const char* msg)
{
	char arg1[DS_ARGV] = "-1";
	char arg2[DS_ARGV] = "<SERVER>";

	char* buff = CombinedMessage(arg1, arg2, msg);
	write(fd, buff, (DS_ARGV * 2) + DS_TEXT);
	delete[] buff;
}

// 메시지를 모든 유저(client)에게 전송
void SendMessage(t_data* data, t_userData* userData, char* arg1, char* arg2,
				 char* msg)
{
	char* buff = CombinedMessage(arg1, arg2, msg);

	for (int i = 0; i < MAX_USER; i++) {
		//접속된 유저이면
		if (userData->fd == i)
			continue;
		if (data->userState[i] == 1 && data->userChannel[i] == atoi(arg1)) {
			write(i, buff, DS_ARGV + DS_ARGV + DS_TEXT);
		}
	}
	delete[] buff;
}

// 유저(client) Fd 에서 이벤트시 호출
void UserEvent(t_data* data, t_userData* userData)
{
	char arg1[DS_ARGV + 1];
	char arg2[DS_ARGV + 1];
	char text[DS_TEXT + 1];

	//데이터 수신
	try {
		readSockFd(userData->fd, arg1, DS_ARGV);
		readSockFd(userData->fd, arg2, DS_ARGV);
		readSockFd(userData->fd, text, DS_TEXT);
		arg1[DS_ARGV] = 0;
		arg2[DS_ARGV] = 0;
		text[DS_TEXT] = 0;
	}
	//읽기 실패시 유저 삭제
	catch (std::string msg) {
		std::cerr << C_ERROR << msg << C_NOMAL << std::endl;
		std::cerr << C_NOMAL << "TIME   : " << pTime() << std::endl;
		std::cerr << C_NOMAL << std::endl;
		RemoveUser(data, userData);
		return;
	}

	//ip 정보 가져오기
	char ipAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(userData->addr.sin_addr), ipAddr, INET_ADDRSTRLEN);

	//서버 메시지 출력
	std::cout << C_NOTIY << "USER   : [" << ipAddr << "]" << C_NOMAL
			  << std::endl;
	std::cout << C_NOMAL << "DATA   : " << arg1 << ", " << arg2 << ", " << text
			  << C_NOMAL << std::endl;
	std::cout << C_NOMAL << "TIME   : " << pTime() << std::endl;
	std::cout << C_NOMAL << std::endl;

	//nick chainge
	if (std::string(arg1) == std::string("/nick")) {
		memcpy(userData->name, arg2, DS_ARGV);

		char temp[DS_TEXT];
		memset(temp, 0, DS_TEXT);
		sprintf(temp, "NickName Changed to : %s", userData->name);
		SendNotification(userData->fd, temp);
	}
	//join channel
	else if (std::string(arg1) == std::string("/join")) {
		userData->ch = atoi(arg2);
		data->userChannel[userData->fd] = atoi(arg2);

		char temp[DS_TEXT];
		memset(temp, 0, DS_TEXT);
		sprintf(temp, "Joind Channel : %d", userData->ch);
		SendNotification(userData->fd, temp);
	}
	//leave channel
	else if (std::string(arg1) == std::string("/leave")) {
		userData->ch = 0;
		data->userChannel[userData->fd] = 0;

		char temp[DS_TEXT];
		memset(temp, 0, DS_TEXT);
		sprintf(temp, "Joind Channel : %d", userData->ch);
		SendNotification(userData->fd, temp);
	}
	//user exit
	else if (std::string(arg1) == std::string("/exit")) {
		char temp[DS_TEXT];
		memset(temp, 0, DS_TEXT);
		sprintf(temp, "See You Again! %s", userData->name);
		SendNotification(userData->fd, temp);

		RemoveUser(data, userData);
	}
	//send msg
	else {
		SendMessage(data, userData, arg1, arg2, text);
	}
}

// 서버(Listen) Fd 에서 이벤트시 호출
void NewUeserEvent(t_data* data)
{
	int userFd;
	t_sockAddrIn addr;
	t_userData* userData;
	t_epollEvent epEvent;

	socklen_t cliLen;

	//user accept()
	cliLen = sizeof(t_sockAddr);
	userFd = accept(data->svFd, (t_sockAddr*)&addr, &cliLen);
	if (userFd == -1) {
		std::cerr << C_ERROR << "ERROR  : accept()" << C_NOMAL << std::endl;
		std::cerr << C_NOMAL << "TIME   : " << pTime() << std::endl;
		std::cerr << C_NOMAL << std::endl;
		return;
	}

	//ip 정보 가져오기
	int port = ntohs(addr.sin_port);
	char ipAddr[INET_ADDRSTRLEN + 1];
	ipAddr[INET_ADDRSTRLEN] = 0;
	inet_ntop(AF_INET, &(addr.sin_addr), ipAddr, INET_ADDRSTRLEN);

	//서버 메시지 출력
	std::cout << C_NOTIY << "SERVER : New User accept()" << C_NOMAL
			  << std::endl;
	std::cout << C_NOMAL << "INFO   : " << ipAddr << ", " << port << C_NOMAL
			  << std::endl;
	std::cout << C_NOMAL << "TIME   : " << pTime() << std::endl;
	std::cout << C_NOMAL << std::endl;

	//option
	int bEnable = 1;
	setsockopt(userFd, SOL_SOCKET, SO_KEEPALIVE, &bEnable, sizeof(int));
	setsockopt(userFd, SOL_SOCKET, SO_REUSEADDR, &bEnable, sizeof(int));
	setsockopt(userFd, IPPROTO_TCP, TCP_NODELAY, &bEnable, sizeof(int));

	//userData 초기화
	userData = new t_userData;
	userData->fd = userFd;
	userData->ch = 0;
	userData->addr = addr;
	memset(userData->name, 0, sizeof(userData->name));

	data->userState[userFd] = 1;	  // 연결 처리
	data->userChannel[userFd] = 0;	  // 체널

	epEvent.events = EPOLLIN;		// 이벤트 들어오면 알림
	epEvent.data.fd = userFd;		// fd 설정
	epEvent.data.ptr = userData;	// 데이터 추가
	//userFd의 상태변화를 감시 대상에 등록
	epoll_ctl(data->epFd, EPOLL_CTL_ADD, userFd, &epEvent);
}
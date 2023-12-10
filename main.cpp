#include "data.hpp"
#include "includes.hpp"

// 연결 된 유저 데이터 (FD는 순차적으로 열림 0,1,2 이후 listenFd, 다음으론 연결된 유저)
int* getUserFds()
{
	static int userFds[MAX_USER];
	return userFds;
}

typedef struct s_svData
{
	t_sockAddrIn addrIn;
	t_epollEvent listenEv;
	t_epollEvent* userEv;
	int epollFd;
	int listenFd;
} t_svData;

// client가 보낸 메시지를 다른 client들에게 전송한다.
void sendMessage(t_epollEvent listenEv, char* msg)
{
	char chBuff[DS_CHANNEL];
	char nameBuff[DS_NAME];
	char textBuff[DS_TEXT];
	memset(textBuff, 0x00, DS_TEXT);
	memset(chBuff, 0x00, DS_CHANNEL);
	memset(nameBuff, 0x00, DS_NAME);

	t_userData* userData = (t_userData*)listenEv.data.ptr;

	//이름과
	sprintf(chBuff, "%d", userData->channel);
	sprintf(nameBuff, "%s", userData->name);
	sprintf(textBuff, "%s", msg);
	for (int i = 0; i < MAX_USER; i++) {
		if (getUserFds()[i] == 1) {
			write(i, chBuff, DS_CHANNEL);
			write(i, nameBuff, DS_NAME);
			write(i, textBuff, DS_TEXT);
		}
	}
}

void userConnectEvent(t_svData& svData)
{
	char buff[DS_TEXT];
	char temp[512];
	int clientFd;
	//버퍼 초기화
	memset(buff, 0, DS_TEXT);

	// accept
	socklen_t clientLen = sizeof(t_sockAddr);
	t_sockAddrIn clientAddr;
	clientFd = accept(svData.listenFd, (t_sockAddr*)&clientAddr, &clientLen);

	// 연결 처리
	getUserFds()[clientFd] = 1;
	t_userData* userData = new t_userData;
	userData->fd = clientFd;

	//이부분은 의논 후 수정해야 함
	{
		//닉네임 설정
		sprintf(temp, "First insert your nickname :");
		write(userData->fd, temp, DS_TEXT);
		sleep(1);

		//닉네임 수신
		read(userData->fd, userData->name, DS_NAME);
		userData->name[DS_NAME - 1] = 0;
	}

	//체널 초기화
	userData->channel = -1;

	//닉네임 확인
	std::cout << "User [" << userData->name << "] has Join" << std::endl;
	sprintf(buff, "Your Nickname : %s\n", userData->name);
	write(userData->fd, buff, DS_TEXT);

	// 이벤트 설정
	svData.listenEv.events = EPOLLIN;
	svData.listenEv.data.ptr = userData;
	// EPOLL_CTL_ADD를 통해 listen 소켓을 이벤트 풀에 추가.
	epoll_ctl(svData.epollFd, EPOLL_CTL_ADD, clientFd, &svData.listenEv);
}

void removeUser(t_svData& svData, t_userData* userData)
{
	epoll_ctl(svData.epollFd, EPOLL_CTL_DEL, userData->fd, svData.userEv);
	close(userData->fd);
	getUserFds()[userData->fd] = -1;
	delete userData;
}

void userChatEvent(t_svData& svData, int i)
{
	int readLen;
	char cmdBuff[DS_CMD];
	char textBuff[DS_TEXT];
	memset(cmdBuff, 0x00, DS_CMD);
	memset(textBuff, 0x00, DS_TEXT);

	//반환된 이벤트 인덱스에서 데이터 가져오기
	t_userData* userData = (t_userData*)svData.userEv[i].data.ptr;

	//명령어 처리
	readLen = read(userData->fd, cmdBuff, DS_CMD);
	// 읽는 도중 에러 발생
	if (readLen <= 0) {
		removeUser(svData, userData);
		return;
	}

	//명령어 처리
	// /msg, /nick, /join 1, /leave 1, /exit 등등

	// 데이터처리 (지금은 모두 /msg로 처리)
	readLen = read(userData->fd, textBuff, DS_TEXT);
	// 읽는 도중 에러 발생
	if (readLen <= 0) {
		removeUser(svData, userData);
		return;
	}
	sendMessage(svData.userEv[i], textBuff);
}

void eventLoop(t_svData& svData)
{
	while (1) {
		// 사건 발생 시까지 무한 대기
		// epollFd의 사건 발생 시 events에 fd를 채운다.
		// eventCnt은 listen에 성공한 fd의 수
		int eventCnt =
			epoll_wait(svData.epollFd, svData.userEv, EPOLL_SIZE, -1);
		if (eventCnt == -1)
			return;
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

int main(int argc, char** argv)
{
	t_svData svData;
	int returnVal;
	socklen_t addrLen;

	// userEv 포인터를 초기화한다. EPOLL_SIZE
	svData.userEv = new t_epollEvent[EPOLL_SIZE];
	// epoll 파일 디스크립터를 만든다. size 는 0 이상이면 됨;
	svData.epollFd = epoll_create(64);
	if ((svData.epollFd) == -1)
		return 1;
	addrLen = sizeof(t_sockAddrIn);
	svData.listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if ((svData.listenFd) == -1)
		return 1;
	svData.addrIn.sin_family = AF_INET;
	svData.addrIn.sin_port = htons(PORT_NBR);
	svData.addrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	returnVal = bind(svData.listenFd, (t_sockAddr*)&svData.addrIn, addrLen);
	if (returnVal == -1)
		return 1;
	returnVal = listen(svData.listenFd, 16);
	if (returnVal == -1)
		return 1;

	// 이벤트 설정
	svData.listenEv.events = EPOLLIN;
	svData.listenEv.data.fd = svData.listenFd;
	// EPOLL_CTL_ADD를 통해 listen 소켓을 이벤트 풀에 추가.
	returnVal = epoll_ctl(svData.epollFd, EPOLL_CTL_ADD, svData.listenFd,
						  &svData.listenEv);
	if (returnVal == -1)
		return 1;
	memset(getUserFds(), 0, sizeof(int) * MAX_USER);
	eventLoop(svData);
}

#include "Incs/Data.hpp"
#include "Incs/Includes.hpp"
#include "Incs/Util.hpp"

char* combinedMessage(char* arg1, char* arg2, const char* msg)
{
	char* dest = new char[DS_ARG + DS_ARG + DS_TEXT];

	for (int i = 0; i < DS_ARG; i++) {
		dest[i] = arg1[i];
		dest[i + DS_ARG] = arg2[i];
	}
	for (int i = 0; i < DS_TEXT; i++) {
		dest[(DS_ARG * 2) + i] = msg[i];
	}
	return dest;
}

// client가 보낸 메시지를 다른 client들에게 전송한다.
void sendMessage(t_svData& svData, char* arg1, char* arg2, char* msg)
{
	int chNum = atoi(arg1);
	char* buff = combinedMessage(arg1, arg2, msg);

	//이름과
	for (int i = 0; i < MAX_USER; i++) {
		t_userData* user = (t_userData*)svData.userEv[i].data.ptr;
		//접속된 유저이며
		if (getUserFds()[i] == 1) {
			//체널에 속한 유저이면 전송
			if (user->channel == chNum || chNum == -777) {
				write(i, buff, DS_ARG + DS_ARG + DS_TEXT);
			}
		}
	}
	delete[] buff;
}

// 서버에서 클라이언트에게 메시지 전송
void sendNotification(int fd, const char* msg)
{
	char arg1[DS_ARG] = "0";
	char arg2[DS_ARG] = "<SERVER>";
	char text[DS_TEXT];
	memset(text, 0, DS_TEXT);
	memcpy(text, msg, DS_TEXT);

	if (getUserFds()[fd] != 1)
		return;
	char* buff = combinedMessage(arg1, arg2, msg);
	write(fd, buff, DS_ARG + DS_ARG + DS_TEXT);
	delete[] buff;
}

void userChatEvent(t_svData& svData, int i)
{
	int readLen;
	char arg1[DS_ARG];
	char arg2[DS_ARG];
	char text[DS_TEXT];

	//반환된 이벤트 인덱스에서 데이터 가져오기
	t_userData* userData = (t_userData*)svData.userEv[i].data.ptr;

	//데이터 수신
	try {
		readSockFd(userData->fd, arg1, DS_ARG);
		readSockFd(userData->fd, arg2, DS_ARG);
		readSockFd(userData->fd, text, DS_TEXT);
	} catch (std::string msg) {
		RemoveUser(svData, userData);
		return;
	}

	//nick chainge
	if (std::string("arg1") == std::string("/nick")) {
		memcpy(userData->name, arg2, DS_ARG);

		std::string temp("nickName Changed to : ");
		temp += userData->name;
		sendNotification(userData->fd, temp.data());
	}
	//join channel
	else if (std::string("arg1") == std::string("/join")) {
		userData->channel = atoi(arg2);

		std::string temp("joind Channel : ");
		temp += userData->channel;
		sendNotification(userData->fd, temp.data());
	}
	//leave channel
	else if (std::string("arg1") == std::string("/leave")) {
		userData->channel = 0;

		std::string temp("joind Channel : ");
		temp += userData->channel;
		sendNotification(userData->fd, temp.data());
	}
	//user exit
	else if (std::string("arg1") == std::string("/exit")) {
		std::string temp("See You Again!");
		sendNotification(userData->fd, temp.data());

		RemoveUser(svData, userData);
	}
	//send msg
	else {
		sendMessage(svData, arg1, arg2, text);
	}
}
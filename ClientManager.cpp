#include "ClientManager.hpp"

ClientManager::ClientManager(void)
{
	this->clientCnt = 0;
}

ClientManager::ClientManager(const ClientManager& src)
{
	*this = src;
}

ClientManager::~ClientManager(void) {}

ClientManager& ClientManager::operator=(ClientManager const& obj)
{
	if (this != &obj) {
		this->clientCnt = obj.clientCnt;
	}
	return *this;
}

void ClientManager::ClientAccessAccept(t_svData& svData)
{
	(void)svData;
	while (1) {
		// struct sockaddr_in clAddr;
		// int accepted_fd;
		// int recv_length;
		// socklen_t size;

		// char buffer[BUF_SIZE];
		// size = sizeof(struct sockaddr_in);
		// accepted_fd = accept(svData.soketFd, (struct sockaddr*)&clAddr, &size);

		// send(accepted_fd, "Connected", 10, 0);
		// printf("Client Info : IP %s, Port %d\n", inet_ntoa(clAddr.sin_addr),
		// 	   ntohs(clAddr.sin_port));

		// recv_length = recv(accepted_fd, &buffer, BUF_SIZE, 0);
		// while (recv_length > 0) {
		// 	printf("From Client : %s\n", buffer);
		// 	recv_length = recv(accepted_fd, &buffer, BUF_SIZE, 0);
		// }
		// close(accepted_fd);
	}
}
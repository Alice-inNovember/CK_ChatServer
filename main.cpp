#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "ClientManager.hpp"
#include "Data.hpp"
#include "Util.hpp"

void InitServerData(t_svData& svData)
{
	//Create Socket
	svData.soketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (svData.soketFd == -1)
		errExit(std::string("socket(), ") + strerror(errno));

	//Data Init
	memset(&svData.svAddr, 0, sizeof(svData.svAddr));
	svData.svAddr.sin_family = AF_INET;
	svData.svAddr.sin_port = htons(PORT);
	svData.svAddr.sin_addr.s_addr = INADDR_ANY;

	//Bind()
	if (bind(svData.soketFd, (struct sockaddr*)&svData.svAddr,
			 sizeof(svData.svAddr)) == -1)
		errExit(std::string("bind(), ") + strerror(errno));

	//Listen()
	if (listen(svData.soketFd, 3) == -1)
		errExit(std::string("listen(), ") + strerror(errno));
}

int main(void)
{
	t_svData svData;
	ClientManager clManager;

	InitServerData(svData);
	clManager.ClientAccessAccept(svData);
	return 0;
}
#ifndef ClientManager_HPP
#define ClientManager_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Data.hpp"
#include "Util.hpp"

class ClientManager
{
   private:
	int clientCnt;

   public:
	ClientManager(void);
	ClientManager(const ClientManager& src);
	virtual ~ClientManager(void);
	ClientManager& operator=(ClientManager const& obj);
	void ClientAccessAccept(t_svData& svData);
};

#endif
#pragma once
#include <WS2tcpip.h>
#include <map>
#include "Common.h"

#define LOBBY_PORT 4000

class Lobby
{
public:
	Lobby();
	void RUN();
	void STOP();
	~Lobby();

private:
	SOCKET lobby_sock_;
	SOCKADDR_IN lobby_addr_;
	std::map<SOCKET, SOCKADDR_IN> clnt_socks_;
	STATE lobby_state_;
};


#pragma once
#include <WS2tcpip.h>
#include <map>
#include <thread>
#include "Common.h"

#define LOBBY_PORT 4000

class Lobby
{
public:
	Lobby();
	~Lobby();
	void Run();
	void Stop();
	void LobbyChat(SOCKET socket_ptr);

private:
	SOCKET lobby_sock_;
	SOCKADDR_IN lobby_addr_;
	std::map<SOCKET, SOCKADDR_IN> clnt_socks_;
	State lobby_state_;
};


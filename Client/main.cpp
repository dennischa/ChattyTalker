#include <WS2tcpip.h>
#include <iostream>
#include "Common.h"
#include "Client.h"
#include "ChatPacket.h"

int main()
{
	WSADATA wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		ErrorHandling("WSAStartup 2.2 failed");
	}

	LobbyClient lobby_clnt;
	lobby_clnt.Connect();
	lobby_clnt.Chat();
	
	WSACleanup();
	return 0;
}
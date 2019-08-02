#include <WS2tcpip.h>
#include <iostream>
#include "Common.h"
#include "Client.h"
#include "ChatPacket.h"

void ShowGuide();

int main()
{
	WSADATA wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		ErrorHandling("WSAStartup 2.2 failed");
	}
	ShowGuide();

	LobbyClient lobby_clnt;
	lobby_clnt.Connect();
	lobby_clnt.Chat();
	
	WSACleanup();
	return 0;
}

void ShowGuide()
{
	std::string in_line;
	std::ifstream in("guide.txt");

	while (std::getline(in, in_line))
	{
		std::cout << in_line << std::endl;
	}

	in.close();
}
#include "Server.h"
using namespace ChattyTalker;

int main()
{
	WSADATA wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		ErrorHandling("WSAStartup 2.2 failed");
	}

	Lobby lobby;
	
	lobby.Run();

	return 0;
}

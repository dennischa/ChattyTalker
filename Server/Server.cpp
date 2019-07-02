#include <iostream>
#include <stdio.h>
#include <WinSock2.h>

int main()
{
	WSADATA wsaData;
	SOCKET clientSock, serverSock;
	SOCKADDR_IN serverAddr, clientAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		return -1;
	}

	serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSock == INVALID_SOCKET)
	{
		WSACleanup();
		return -1;
	}

	WSACleanup();
	return -1;
}

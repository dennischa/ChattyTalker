#include "Common.h"

void ErrorHandling(const char* message, SOCKET* socket)
{
	printf("%s, Error Code: %d\n", message, WSAGetLastError());
	if (socket != nullptr)
		closesocket(*socket);
	WSACleanup();
	system("PAUSE");
	exit(1);
}

std::string toString(SOCKADDR_IN& addr)
{
	std::string str;
	char buf[20];
	inet_ntop(AF_INET, &addr.sin_addr.S_un.S_addr, buf, sizeof(buf));

	str = buf;

	str += ":" + std::to_string(ntohs(addr.sin_port));

	return str;
}

bool Equal(SOCKADDR_IN laddr, SOCKADDR_IN raddr)
{
	if (laddr.sin_family != raddr.sin_family)
		return false;

	if (laddr.sin_port != raddr.sin_port)
		return false;
	
	if (laddr.sin_addr.S_un.S_addr != raddr.sin_addr.S_un.S_addr)
		return false;

	return true;
}

bool IsWSAEWOULDBLOCK(const int result)
{
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}

	return false;
}

bool SetNonBlockMode(SOCKET& socket)
{
	u_long non_block_mode = 1;

	int iRsult = ioctlsocket(socket, FIONBIO, &non_block_mode);

	if (iRsult == SOCKET_ERROR)
	{
		ErrorHandling("SetNonBlockMode : ioctlsocket failed", &socket);
	}

	return true;
}
#include "Common.h"

void ErrorHandling(const char* message, SOCKET* socket)
{
	printf("%s, Error Code: %d\n", message, WSAGetLastError());
	if (socket != nullptr)
		closesocket(*socket);
	WSACleanup();
	exit(1);
}
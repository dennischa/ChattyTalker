#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iostream>

using namespace std;

int main()
{
	WSADATA wsadata;
	SOCKET clnt_socket;
	SOCKADDR_IN serv_addr;

	int strLen;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		cout << "WSAStartup error" << endl;
		return -1;
	}

	clnt_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clnt_socket == INVALID_SOCKET)
	{
		cout << "Invalid socket" << endl;
		WSACleanup();
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1",&serv_addr.sin_addr.S_un.S_addr);
	serv_addr.sin_port = htons(4000); //임의 포트 번호 4000

	if (connect(clnt_socket, (SOCKADDR*)& serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		cout << "connecting socking failed : "  << WSAGetLastError() << endl;
		closesocket(clnt_socket);
		WSACleanup();
		return -1;
	}

	char message[100];

	recv(clnt_socket, message, 100, 0);
	
	cout << message << endl;

	WSACleanup();
	return 0;
}

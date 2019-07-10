#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	WSADATA wsadata;
	SOCKET lis_sock, accp_sock;
	SOCKADDR_IN serv_addr, clnt_addr;
	int clnt_addr_size;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		cout << "WSAStartup error" << endl;
		return -1;
	}

	lis_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //TCP listen socket 생성

	if (lis_sock == INVALID_SOCKET)
	{
		cout << "Invalid socket" << endl;
		WSACleanup();
		return -1;
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET; //주소 체계, IPv4
	serv_addr.sin_port = htons(4000); //임의 포트 번호 4000
	serv_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //운영체제가 부여한 주소

	if (bind(lis_sock, (SOCKADDR*) & serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		cout << "binding socket failed Error Code: " << WSAGetLastError() << endl;
		closesocket(lis_sock);
		WSACleanup();
		return -1;
	}

	if (listen(lis_sock, SOMAXCONN) == SOCKET_ERROR) //연결 대기열 크기 설정 
	{
		cout << "listening socking failed" << endl;
		closesocket(lis_sock);
		WSACleanup();
		return -1;
	}

	cout << "Listening..." << endl;

	clnt_addr_size = sizeof(clnt_addr);
	accp_sock = accept(lis_sock, (SOCKADDR*)& clnt_addr, &clnt_addr_size);

	if (accp_sock == INVALID_SOCKET)
	{
		cout << "Invalid socket" << endl;
		closesocket(accp_sock);
		closesocket(lis_sock);
		WSACleanup();
		return -1;
	}

	char message[100] = "Server Connected";

	char addr[10];

	inet_ntop(AF_INET, &clnt_addr.sin_addr.S_un.S_addr, addr, sizeof(addr));
	cout << "Connected : " << addr << endl;
	cout << sizeof(message) << endl;

	send(accp_sock, message, sizeof(message), 0);

	closesocket(lis_sock);
	closesocket(accp_sock);
	WSACleanup();
	return 0;
}

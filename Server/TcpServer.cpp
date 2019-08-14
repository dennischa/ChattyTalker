#include "Server.h"

using namespace ChattyTalker;

TcpServer::TcpServer()
{
	serv_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("TcpServer::TcpServer(): Invalid serv_socket", &serv_sock_);
	}

	serv_addr_.sin_family = AF_INET;
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);
}

void TcpServer::Stop()
{
	std::map<SOCKET, SOCKADDR_IN>::iterator index = clnt_socks_.begin();

	for (; index != clnt_socks_.end(); index++)
	{
		closesocket((*index).first);
	}

	Server::Stop();
}
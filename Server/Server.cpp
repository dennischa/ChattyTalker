#include "Server.h"

Server::Server()
{
	serv_state_ = STOPPED;
}

Server::~Server()
{
	Stop();
}

void Server::Stop()
{
	if (serv_state_ == RUNNING)
	{
		closesocket(serv_sock_);

		std::map<SOCKET, SOCKADDR_IN>::iterator index = clnt_socks_.begin();
		for (; index != clnt_socks_.end(); index++)
		{
			closesocket((*index).first);
		}

		serv_state_ = STOPPED;
	}
}
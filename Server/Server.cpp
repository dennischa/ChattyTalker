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
	closesocket(serv_sock_);

	std::map<SOCKET, SOCKADDR_IN>::iterator index = clnt_socks_.begin();
	for (; index != clnt_socks_.end(); index++)
	{
		closesocket((*index).first);
	}

	serv_state_ = STOPPED;

	for (int i = 0; i < threads.size(); i++)
	{
		std::thread& thread = threads.at(i);
		if (thread.joinable())
		{
			//printf("Server::Stop() Wait Thread Id : %p", thread.get_id());
			thread.join();
		}
	}
}
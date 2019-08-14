#include "Server.h"

using namespace ChattyTalker;

Server::Server()
{
	serv_state_ = STOPPED;
	memset(&serv_addr_, 0, sizeof(serv_addr_));
}

Server::~Server()
{
	Stop();
}

void Server::Stop()
{
	closesocket(serv_sock_);

	serv_state_ = STOPPED;

	for (int i = 0; i < threads.size(); i++)
	{
		std::thread& thread = threads.at(i);
		if (thread.joinable())
		{
			thread.join();
		}
	}
}
#include "Server.h"

using namespace ChattyTalker;

UdpServer::UdpServer()
{
	serv_sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("UdpServer::UdpServer() : Invalid serv_socket", &serv_sock_);
	}

	serv_addr_.sin_family = AF_INET;
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

}

bool UdpServer::Join(SOCKADDR_IN addr)
{
	std::vector<SOCKADDR_IN>::iterator it = clnt_addrs_.begin();
	for (; it != clnt_addrs_.end(); it++)
	{
		if (Equal(*it, addr))
		{
			return false; // already joined
		}
	}

	clnt_addrs_.push_back(addr);
	return true;
}

bool UdpServer::Leave(SOCKADDR_IN addr)
{
	std::vector<SOCKADDR_IN>::iterator it = clnt_addrs_.begin();
	for (; it != clnt_addrs_.end(); it++)
	{
		if (Equal(*it, addr))
		{
			clnt_addrs_.erase(it);
			return true;
		}
	}
	return false;
}
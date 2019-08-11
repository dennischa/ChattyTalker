#include "Server.h"

NonBlockUdpServ::NonBlockUdpServ()
{
	serv_sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("NonBlockUdpServ : Invalid serv socket", &serv_sock_);
	}

	memset(&serv_addr_, 0, sizeof(serv_addr_));

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(NBLOCK_UDP_PORT);
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("NonBlockUdpServ : Binding serv_sock failed\n", &serv_sock_);
	}

	SetNonBlockMode(serv_sock_);
}

void NonBlockUdpServ::Run()
{
	printf("NonBlockUdpServ::Run() Start\n");

	serv_state_ = RUNNING;

	char buf[MAX_PACKET_SIZE];
	std::vector<SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(buf, 0, sizeof(buf));

		int result = recvfrom(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)& clnt_addr, &addrlen);

		if (IsWSAEWOULDBLOCK(result))
		{
			//It would be blocked, didn't recv packet
			continue;
		}
		else if(result == SOCKET_ERROR)
		{
			ErrorHandling("NonBlockUdpServ::Run : recvfrom failed");
		}

		printf("recvfrom : %s\n", toString(clnt_addr).c_str());

		ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == MESSAGE)
		{
			for (it = clnt_addrs_.begin(); it != clnt_addrs_.end(); it++)
			{
				SOCKADDR_IN addr = *it;
				if ((clnt_addr.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) || (clnt_addr.sin_port != addr.sin_port))
				{
					sendto(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)& addr, sizeof(addr));
					printf("NonBlockUdpServ::Run : sendto %s\n", toString(addr).c_str());
				}
			}
		}
		else
		{
			ErrorHandling("NonBlockUdpServ::Run : wrong packet type");
		}
	}
}

bool NonBlockUdpServ::Join(SOCKADDR_IN addr)
{
	clnt_addrs_.push_back(addr);
	printf("NonBlockUdpServ::Join Port Number : %d\n", ntohs(addr.sin_port));
	return true;
}

bool NonBlockUdpServ::Leave(SOCKADDR_IN addr)
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
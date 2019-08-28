#include "Server.h"

using namespace ChattyTalker;

NonBlockUdpServ::NonBlockUdpServ()
{
	serv_addr_.sin_port = htons(NBLOCK_UDP_PORT);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("NonBlockUdpServ : Binding serv_sock failed\n", &serv_sock_);
	}

	SetNonBlockMode(serv_sock_);
}

void NonBlockUdpServ::Run()
{
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

		ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == MESSAGE)
		{
			for (it = clnt_addrs_.begin(); it != clnt_addrs_.end(); it++)
			{
				SOCKADDR_IN addr = *it;
				if ((clnt_addr.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) || (clnt_addr.sin_port != addr.sin_port))
				{
					sendto(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)& addr, sizeof(addr));
				}
			}
		}
		else
		{
			ErrorHandling("NonBlockUdpServ::Run : wrong packet type");
		}
	}
}
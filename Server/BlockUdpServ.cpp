#include "Server.h"

using namespace ChattyTalker;

BlockUdpServ::BlockUdpServ()
{
	serv_addr_.sin_port = htons(BLOCK_UDP_PORT);
	
	if (bind(serv_sock_, (SOCKADDR*)&serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockUdpServ : Binding serv_sock failed\n", &serv_sock_);
	}
}

void BlockUdpServ::Run()
{
	serv_state_ = RUNNING;
	
	char buf[MAX_PACKET_SIZE];
	std::vector<SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(buf, 0, sizeof(buf));

		int r = recvfrom(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)&clnt_addr, &addrlen);

		ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == MESSAGE)
		{ 
			for (it = clnt_addrs_.begin(); it != clnt_addrs_.end(); it++)
			{
				SOCKADDR_IN addr = *it;
				if ((clnt_addr.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) || (clnt_addr.sin_port != addr.sin_port))
				{
					sendto(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)&addr, sizeof(addr));
				}
			}
		}
		else
		{
			ErrorHandling("BlockUdpServ::Run : wrong packet type");
		}
	}
}

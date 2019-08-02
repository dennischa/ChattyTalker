#include "Server.h"

BlockUdpServ::BlockUdpServ()
{
	serv_sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("BlockUdpServ: Invalid serv socket", &serv_sock_);
	}

	memset(&serv_addr_, 0, sizeof(serv_addr_));

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(BLOCK_UDP_PORT);
	serv_addr_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(serv_sock_, (SOCKADDR*)&serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockUdpServ: Binding serv_sock failed", &serv_sock_);
	}

	printf("BlockUdpServ Port Number : %d\n", serv_addr_.sin_port);
}

void BlockUdpServ::Run()
{
	printf("BlockUdpServ::Run() Start");

	serv_state_ = RUNNING;
	
	char buf[MAX_PACKET_SIZE];
	std::vector<SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		SOCKADDR_IN clnt_addr;
		memset(buf, 0, sizeof(buf));

		int r = recvfrom(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)& clnt_addr, nullptr);
		//int r = recvfrom(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)& clnt_addr, nullptr);
		printf("BlckUdp listen %s\n", buf);
		/*ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == MESSAGE)
		{
			printf("BlockUdpServ::Run() : Message %s\n", ((MessagePacket*)buf)->get_message());
 
			for (it = clnt_addrs_.begin(); it != clnt_addrs_.end(); it++)
			{
				SOCKADDR_IN addr = *it;
				if (clnt_addr.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr)
				{
					sendto(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)&addr, sizeof(addr));
				}
			}
		}
		else if (chat_packet->get_packet_type() == LEAVE)
		{
			//LEAVE
		}
		else
		{
			//ERROR
		}*/
	}
}

bool BlockUdpServ::Join(SOCKADDR_IN addr)
{
	clnt_addrs_.push_back(addr);
	return true;
}

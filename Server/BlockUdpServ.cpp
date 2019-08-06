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
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)&serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockUdpServ: Binding serv_sock failed\n", &serv_sock_);
	}

	printf("BlockUdpServ Port Number : %d\n", serv_addr_.sin_port);

	SOCKADDR_IN addr;
	int len = sizeof(addr);
	int sockname = getsockname(serv_sock_, (SOCKADDR*)& addr, &len);
	if (sockname != 0)
	{
		ErrorHandling("udp clnt sockt addr error");
	}
	else
	{
		printf("serv_scok name, %s\n", toString(addr).c_str());
	}
}

void BlockUdpServ::Run()
{
	printf("BlockUdpServ::Run() Start\n");

	serv_state_ = RUNNING;
	
	char buf[MAX_PACKET_SIZE];
	std::vector<SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(buf, 0, sizeof(buf));

		int r = recvfrom(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)&clnt_addr, &addrlen);
		
		printf("recvfrom : %s\n", toString(clnt_addr).c_str());

		ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == MESSAGE)
		{ 
			for (it = clnt_addrs_.begin(); it != clnt_addrs_.end(); it++)
			{
				SOCKADDR_IN addr = *it;
				if ((clnt_addr.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) || (clnt_addr.sin_port != addr.sin_port))
				{
					sendto(serv_sock_, buf, sizeof(buf), 0, (SOCKADDR*)&addr, sizeof(addr));
					printf("BlockUdpSe sendto : %s\n", toString(addr).c_str());
				}
			}
		}
		else if (chat_packet->get_packet_type() == LEAVE)
		{
			//LEAVE
			Leave(clnt_addr);
		}
		else
		{
			ErrorHandling("BlockUdpServ::Run : wrong packet type");
		}
	}
}

bool BlockUdpServ::Join(SOCKADDR_IN addr)
{
	clnt_addrs_.push_back(addr);
	printf("BlockUdpServ::Join Port Number : %d\n", ntohs(addr.sin_port));
	return true;
}

bool BlockUdpServ::Leave(SOCKADDR_IN addr)
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

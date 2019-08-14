#include "Server.h"

using namespace ChattyTalker;

BlockTcpServ::BlockTcpServ()
{
	serv_addr_.sin_port = htons(BLOCK_TCP_PORT);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpServ: Binding serv_sock failed", &serv_sock_);
	}
}

void BlockTcpServ::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpServ: Listening serv_socket failed", &serv_sock_);
	}

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);

		accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);
		if (accp_sock == INVALID_SOCKET)
		{
			ErrorHandling("BlockTcpServ: Invalid accp_socket", &accp_sock);
		}

		clnt_socks_[accp_sock] = clnt_addr;

		printf("BlockTcpServ : Accept Client Connect : %s\n", toString(clnt_addr).c_str());

		//클라이언트랑 대화하는 쓰레드 생성
		//std::thread chat([&]() { Chat(accp_sock); });
		std::thread chat(&BlockTcpServ::Chat, this, accp_sock);
		threads.push_back(std::move(chat));
	}
}

void BlockTcpServ::Chat(SOCKET socket)
{
	char buf[MAX_PACKET_SIZE];
	std::map<SOCKET, SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int r = recv(socket, buf, MAX_PACKET_SIZE, 0);

		if (r <= 0)
		{
			it = clnt_socks_.find(socket);
			if (it != clnt_socks_.end())
				clnt_socks_.erase(it);

			return;
		}

		PacketType packet_type;
		
		if (GetPacketType(buf, packet_type))
		{
			if (packet_type == MESSAGE)
			{
				it = clnt_socks_.begin();
				for (; it != clnt_socks_.end(); it++)
				{
					SOCKET clnt_sock = (it->first);
					if (clnt_sock != socket)
					{
						if (clnt_sock != socket)
							send(clnt_sock, buf, sizeof(buf), 0);
					}
				}
			}
			else
			{
				ErrorHandling("BlockTcpServ : Wrong packet type");
			}
		}
	}
}
#include "Server.h"

Lobby::Lobby()
{
	serv_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("Lobby: Invalid lobby socket", &serv_sock_);
	}

	memset(&serv_addr_, 0, sizeof(serv_addr_));

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(LOBBY_PORT);
	serv_addr_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
}
void Lobby::Run()
{
	serv_state_ = RUNNING;

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Binding lobby socket failed", &serv_sock_);
	}

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Listening lobby socket failed", &serv_sock_);
	}

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;

		accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, nullptr);
		if (accp_sock == INVALID_SOCKET)
		{
			ErrorHandling("Lobby: Invalid accp_socket", &accp_sock);
		}

		clnt_socks_[accp_sock] = clnt_addr;
		char addr_in[20];
		inet_ntop(AF_INET, &clnt_addr.sin_addr.S_un.S_addr, addr_in, sizeof(addr_in));
		printf("Connected %s\n", addr_in);

		//클라이언트랑 대화하는 쓰레드 생성
		std::thread lobby_chat([&]() {Chat(accp_sock); });
		lobby_chat.detach();
	}
}


void Lobby::Chat(SOCKET socket)
{
	char buf[MAX_PACKET_SIZE];

	while (true)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int r = recv(socket, buf, MAX_PACKET_SIZE, 0);

		if (r <= 0)
		{
			printf("Lobby: connection close\n");
			
			std::map<SOCKET, SOCKADDR_IN>::iterator it = clnt_socks_.find(socket);
			if (it != clnt_socks_.end())
				clnt_socks_.erase(it);

			return;
		}

		ChatPacket* chat_packet = (ChatPacket*)buf;

		if (chat_packet->get_packet_type() == JOIN)
		{
			JoinPacket* join_packet = (JoinPacket*)buf;
			RoomType room_type = join_packet->get_room_type();

			printf("Lobby : Join ");
			switch (room_type)
			{
			case BLOCK_UDP:
			{
				//make udp serv
				if (chat_rooms_.find(BLOCK_UDP) == chat_rooms_.end())
				{
					chat_rooms_[BLOCK_UDP] = new BlockUdpServ;
					std::thread chat([&]() {chat_rooms_[BLOCK_UDP]->Run(); });
					chat.detach();
				}
				BlockUdpServ* serv = (BlockUdpServ*)chat_rooms_[BLOCK_UDP];

				printf("Host Packet Port number : %d\n", serv->get_serv_addr().sin_port);
				//send host info
				HostInfoPacket host_info_packet(serv->get_serv_addr(), BLOCK_UDP);
				send(socket, (char*)&host_info_packet, sizeof(host_info_packet), 0);

				//Join
				serv->Join(clnt_socks_[socket]);
				break;
			}
			/*case BLOCK_TCP:
				break;
			case NONBLOCK_UDP:
				break;
			case NONBLOCK_TCP:
				break;*/
			}
		}
		else
		{
			//에러 메세지
		}
	}
}

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
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Binding lobby socket failed", &serv_sock_);
	}
}
void Lobby::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Listening lobby socket failed", &serv_sock_);
	}

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);

		accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);
		if (accp_sock == INVALID_SOCKET)
		{
			ErrorHandling("Lobby: Invalid accp_socket", &accp_sock);
		}

		clnt_socks_[accp_sock] = clnt_addr;
		
		printf("Lobby : Accept Client Connect : %s\n", toString(clnt_addr).c_str());

		//클라이언트랑 대화하는 쓰레드 생성
		//std::thread lobby_chat([&]() {Chat(accp_sock); });
		std::thread lobby_chat(&Lobby::Chat, this, accp_sock);
		threads.push_back(std::move(lobby_chat));
		//lobby_chat.detach();
	}
}


void Lobby::Chat(SOCKET socket)
{
	char buf[MAX_PACKET_SIZE];

	while (serv_state_ == RUNNING)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int r = recv(socket, buf, MAX_PACKET_SIZE, 0);

		if (r <= 0)
		{
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

			Server* serv = FindChatroom(room_type);
			
			switch (room_type)
			{
			case BLOCK_UDP:
			case NONBLOCK_UDP:
			{
				//send serv info
				HostInfoPacket serv_info(serv->get_serv_addr(), room_type);
				send(socket, (char*)& serv_info, sizeof(serv_info), 0);

				//recv clnt info
				memset(buf, 0, MAX_PACKET_SIZE);
				int r = recv(socket, (char*)buf, MAX_PACKET_SIZE, 0);

				HostInfoPacket* clnt_info = (HostInfoPacket*)buf;
				if (clnt_info->get_packet_type() != HOST_INFO)
				{
					ErrorHandling("Lobby::Chat : must be HostInfo type");
				}

				BlockUdpServ* blck_udp = (BlockUdpServ*)serv;
				blck_udp->Join(clnt_info->get_host_addr());
				break;
			}
			case BLOCK_TCP:
			case NONBLOCK_TCP:
			{
				//send serv info
				HostInfoPacket serv_info(serv->get_serv_addr(), room_type);
				send(socket, (char*)& serv_info, sizeof(serv_info), 0);

				break;
			}
			/*case NONBLOCK_UDP:
			{
				HostInfoPacket serv_info(serv->get_serv_addr(), NONBLOCK_UDP);
				send(socket, (char*)& serv_info, sizeof(serv_info), 0);

				memset(buf, 0, MAX_PACKET_SIZE);
				int r = recv(socket, (char*)buf, MAX_PACKET_SIZE, 0);
				if (r < 0)
				{
					ErrorHandling("Lobby NONBLOCK_UDP : failed recv hostInfo ");
					return;
				}

				HostInfoPacket* clnt_info = (HostInfoPacket*)buf;
				if (clnt_info->get_packet_type() != HOST_INFO)
				{
					ErrorHandling("Lobby BLOCK_UDP : wrong packet type");
				}

				NonBlockUdpServ* nblck_udp = (NonBlockUdpServ*)serv;
				nblck_udp->Join(clnt_info->get_host_addr());

				break;
			}*/
			default:
			{
				ErrorHandling("Lobby::Run : wrong room type");
			}
			}
		}
		else if (chat_packet->get_packet_type() == LEAVE)
		{
			LeavePacket* leave_packet = (LeavePacket*)buf;
			RoomType room_type = leave_packet->get_room_type();
			SOCKADDR_IN addr = leave_packet->get_addr();

			switch (room_type)
			{
			case BLOCK_UDP:
			{
				BlockUdpServ* serv = (BlockUdpServ*)FindChatroom(BLOCK_UDP);
				serv->Leave(addr);
				break;
			}
			case NONBLOCK_UDP:
			{
				NonBlockUdpServ* serv = (NonBlockUdpServ*)FindChatroom(NONBLOCK_UDP);
				serv->Leave(addr);
				break;
			}
			default:
				ErrorHandling("Lobby::Chat Leave packet : wrong room type");
				break;
			}
		}
		else
		{
			//에러 메세지
		}
	}
}

Server* Lobby::FindChatroom(const RoomType room_type)
{
	if (chat_rooms_.find(room_type) == chat_rooms_.end())
	{
		Server* server = nullptr;
		switch (room_type)
		{
		case BLOCK_UDP:
			server = new BlockUdpServ;
			break;
		case BLOCK_TCP:
			server = new BlockTcpServ;
			break;
		case NONBLOCK_UDP:
			server = new NonBlockUdpServ;
			break;
		default:
			ErrorHandling("Lobby::FindChatroom : Add Chat Room Type");
			break;
		}
		chat_rooms_[room_type] = server;

		std::thread run(&Server::Run, server);
		threads.push_back(move(run));
	}

	return chat_rooms_[room_type];
}

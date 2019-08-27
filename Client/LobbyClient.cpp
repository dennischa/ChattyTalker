#include "Client.h"

LobbyClient::LobbyClient()
{
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);
	serv_addr_.sin_port = htons(LOBBY_PORT);
}

bool LobbyClient::Connect()
{
	if (connect(clnt_socket_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("LobbyClient : Connect socket error", &clnt_socket_);
		return false;
	}

	return true;
}

void LobbyClient::Chat()
{
	char message[MAX_MESSAGE_SIZE];

	ShowGuide();

	while (true)
	{
		memset(message, 0, MAX_MESSAGE_SIZE);
		std::cin.getline(message, MAX_MESSAGE_SIZE);

		int type = atoi(message);

		if (type > 0 && type < 8)
		{
			printf("Lobby: Join %d\n", type);
			JoinPacket joinPacket((RoomType)type);
			send(clnt_socket_, (const char*)& joinPacket, sizeof(joinPacket), 0);

			//recv host info
			char buf[MAX_PACKET_SIZE];
			memset(buf, 0, MAX_PACKET_SIZE);

			int r = recv(clnt_socket_, buf, MAX_PACKET_SIZE, 0);
			ChatPacket* chat_packet = (ChatPacket*)buf;

			if (chat_packet->get_packet_type() == HOST_INFO)
			{
				HostInfoPacket* host_info_packet = (HostInfoPacket*)buf;
				if (type != host_info_packet->get_room_type())
				{
					ErrorHandling("LobbyClient : discordance between host types");
				}

				SOCKADDR_IN serv_addr = host_info_packet->get_host_addr();

				RoomType r_type = (RoomType)type;

				switch (r_type)
				{
				case BLOCK_UDP:
				case NONBLOCK_UDP:
				{
					UdpClient* udp_ptr = (UdpClient * )MakeClient(r_type, serv_addr);
					SOCKADDR_IN addr = udp_ptr->get_clnt_addr();
					
					HostInfoPacket clnt_info(addr, r_type);
					send(clnt_socket_, (char*)& clnt_info, sizeof(clnt_info), 0);
					
					udp_ptr->Chat();

					LeavePacket leave_packet(r_type, addr);
					send(clnt_socket_, (char*)& leave_packet, sizeof(leave_packet), 0);

					delete udp_ptr;
					
					break;
				}
				case BLOCK_TCP:
				case NONBLOCK_TCP:
				{
					TcpClient* tcp_ptr = (TcpClient*)MakeClient(r_type, serv_addr);
					
					if (tcp_ptr->Connect())
					{
						tcp_ptr->Chat();
					}

					delete tcp_ptr;

					break;
				}
				case OVERLLAPED_IO:
				case IOCP:
				{
					OverlappedClient* ov_clnt_ptr = (OverlappedClient*)MakeClient(r_type, serv_addr);

					if (ov_clnt_ptr->Connect())
					{
						ov_clnt_ptr->Chat();
					}

					delete ov_clnt_ptr;
					break;
				}
				default:
				{
					ErrorHandling("LobbyClient::Chat : worng room type");
					break;
				}
				}

				ShowGuide();
			}
			else
			{
				printf("Wrong Packet\n");
			}
		}
		else
		{
			printf("Wrong Input\n");
		}
	}
}

Client* LobbyClient::MakeClient(RoomType type, SOCKADDR_IN addr)
{
	Client* clnt_ptr = nullptr;
	switch (type)
	{
	case BLOCK_UDP:
	{
		clnt_ptr = new BlockUdpClient(addr);
		break;
	}
	case BLOCK_TCP:
	{
		clnt_ptr = new BlockTcpClient(addr);
		break;
	}
	case NONBLOCK_UDP:
	{
		clnt_ptr = new NonBlockUdpClient(addr);
		break;
	}
	case NONBLOCK_TCP:
	{
		clnt_ptr = new NonBlockTcpClient(addr);
		break;
	}
	case OVERLLAPED_IO:
	case IOCP:
	{
		clnt_ptr = new OverlappedClient(addr);
		break;
	}
	}

	return clnt_ptr;
}

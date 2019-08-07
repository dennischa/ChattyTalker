#include "Client.h"

LobbyClient::LobbyClient()
{
	clnt_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("LobbyClient : Invalid socket", &clnt_socket_);
	}
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
				//make client
				HostInfoPacket* host_info_packet = (HostInfoPacket*)buf;
				if (type != host_info_packet->get_room_type())
				{
					//Error
					ErrorHandling("LobbyClient : discordance between host types");
				}

				switch (type)
				{
				case BLOCK_UDP:
				{
					BlockUdpClient blck_udp_clnt(host_info_packet->get_host_addr());
					SOCKADDR_IN addr;
					int len = sizeof(addr);
					int sockname = getsockname(blck_udp_clnt.get_clnt_socket(), (SOCKADDR*)& addr, &len);
					if (sockname != 0)
					{
						ErrorHandling("udp clnt sockt addr error");
					}
					HostInfoPacket clnt_info(addr, BLOCK_UDP);
					send(clnt_socket_, (char*)& clnt_info, sizeof(clnt_info), 0);
					blck_udp_clnt.Chat();
					break;
				}
				case BLOCK_TCP:
				{
					BlockTcpClient blck_tcp_clnt(host_info_packet->get_host_addr());
					break;
				}
			}

			ShowGuide();
		}
		else
		{
			printf("Wrong Input\n");
		}
	}
}

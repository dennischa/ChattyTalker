#include "Client.h"

bool BlockTcpClient::Connect()
{
	if (connect(clnt_socket_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpClient : Connect socket error", &clnt_socket_);
		return false;
	}

	return true;
}

void BlockTcpClient::Chat()
{
	char msg[MAX_MESSAGE_SIZE];
	bool on_chat = true;

	//std::thread recv([&]() { Recv(on_chat); });
	std::thread recv(&BlockTcpClient::Recv, this, std::ref(on_chat));

	while (on_chat)
	{
		memset(msg, 0, MAX_MESSAGE_SIZE);
		std::cin.getline(msg, MAX_MESSAGE_SIZE);

		if (strlen(msg) < 1)
			continue;

		if (strcmp(msg, "/exit") == 0)
		{
			break;
		}

		MessagePacket msg_packet(msg);

		send(clnt_socket_, (char*)& msg_packet, sizeof(msg_packet), 0);
	}

	closesocket(clnt_socket_);
	
	on_chat = false;
	
	if(recv.joinable())
		recv.join();
}

void BlockTcpClient::Recv(bool& on_chat)
{
	char buf[MAX_PACKET_SIZE];
	
	while (on_chat)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int r = recv(clnt_socket_, buf, MAX_PACKET_SIZE, 0);

		if (r < 0)
		{
			printf("Disconnet from Server");
			on_chat = false;
			return;
		}

		PacketType packet_type;
		if (GetPacketType(buf, packet_type) == MESSAGE)
		{
			MessagePacket* msg_packet = (MessagePacket*)buf;
			printf("%s\n", msg_packet->get_message());
		}
	}
}
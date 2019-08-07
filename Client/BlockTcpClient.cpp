#include "Client.h"

BlockTcpClient::BlockTcpClient(SOCKADDR_IN serv_addr)
{
	clnt_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("BlockTcpClient : Invalid clnt socket", &clnt_socket_);
	}

	serv_addr_ = serv_addr;

	/*SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	getsockname(clnt_socket_, (SOCKADDR*)& addr, &addrlen);
	printf("BlockTcpClient : %s\n", toString(addr).c_str());*/
}

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

	std::thread recv([&]() { Recv(on_chat); });

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
	if(recv.joinable())
		recv.join();
	on_chat = false;
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
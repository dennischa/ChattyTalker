#include "Client.h"

BlockUdpClient::BlockUdpClient(SOCKADDR_IN serv_addr)
{
	clnt_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("BlockUdpClient : Invalid clnt_sock_ ", &clnt_socket_);
	}

	serv_addr_ = serv_addr;
	printf("block udp Port : %d\n", serv_addr_.sin_port);
}

void BlockUdpClient::Chat()
{
	printf("BlockUdpClient::Chat() Start\n");

	char message[MAX_MESSAGE_SIZE];

	while (true)
	{
		memset(message, 0, MAX_MESSAGE_SIZE);
		std::cin.getline(message, MAX_MESSAGE_SIZE);

		if (strlen(message) < 1)
			continue;

		//Leave 처리 필요

		MessagePacket msg_packet(message);
		printf("%s\n", msg_packet.get_message());
		sendto(clnt_socket_, (char*)message, sizeof(message), 0, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_));
	}
}
#include "Client.h"

void BlockUdpClient::Chat()
{
	//printf("BlockUdpClient::Chat() Start\n");
	bool on_chat = true;
	//std::thread recvfrom([&]() { Recvfrom(on_chat); });
	std::thread recvfrom(&BlockUdpClient::Recvfrom, this, std::ref(on_chat));

	char message[MAX_MESSAGE_SIZE];

	while (true)
	{
		memset(message, 0, MAX_MESSAGE_SIZE);
		std::cin.getline(message, MAX_MESSAGE_SIZE);

		if (strlen(message) < 1)
			continue;

		if (strcmp(message, "/exit") == 0)
		{
			on_chat = false;
			closesocket(clnt_socket_);
			recvfrom.join();
			return;
		}

		MessagePacket msg_packet(message);

		int result = sendto(clnt_socket_, (char*)&msg_packet, sizeof(msg_packet), 0, (SOCKADDR*) &serv_addr_, sizeof(serv_addr_));
	}
}

void BlockUdpClient::Recvfrom(bool& on_chat)
{
	char buf[MAX_PACKET_SIZE];
	while (on_chat)
	{
		memset(buf, 0, MAX_PACKET_SIZE);
		SOCKADDR_IN addr;
		int addrlen = sizeof(addr);

		int r = recvfrom(clnt_socket_, buf, MAX_PACKET_SIZE, 0, (SOCKADDR*)& addr, &addrlen);

		if (r < 0)
		{
			return;
		}

		PacketType packet_type;
		if (GetPacketType(buf, packet_type) && packet_type == MESSAGE)
		{
			MessagePacket* msg_packet = (MessagePacket*)buf;
			printf("%s\n", msg_packet->get_message());
		}
	}
}
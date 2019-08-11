#include "Client.h"

NonBlockUdpClient::NonBlockUdpClient(SOCKADDR_IN serv_addr)
{
	clnt_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("NonBlockUdpClient : Invalid clnt_sock_ ", &clnt_socket_);
	}

	serv_addr_ = serv_addr;

	if (bind(clnt_socket_, (SOCKADDR*)& clnt_addr_, sizeof(clnt_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("Client: binding socket failed", &clnt_socket_);
	}

	SetNonBlockMode(clnt_socket_);
}

void NonBlockUdpClient::Chat()
{
	bool on_chat = true;

	std::thread recvfrom(&NonBlockUdpClient::Recvfrom, this, std::ref(on_chat));

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

		while (true)
		{
			int result = sendto(clnt_socket_, (char*)& msg_packet, sizeof(msg_packet), 0, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_));

			if (IsWSAEWOULDBLOCK(result))
			{
				//It would be blocked, didn't send message
				continue;
			}
			else if(result > 0)
			{
				//Success to send
				break;
			}
			else
			{
				ErrorHandling("NonBlockUdpClient::Chat : sendto failed");
			}
		}
	}
}

void NonBlockUdpClient::Recvfrom(bool& on_chat)
{
	char buf[MAX_PACKET_SIZE];
	while (on_chat)
	{
		memset(buf, 0, MAX_PACKET_SIZE);
		SOCKADDR_IN addr;
		int addrlen = sizeof(addr);

		int result = recvfrom(clnt_socket_, buf, MAX_PACKET_SIZE, 0, (SOCKADDR*)& addr, &addrlen);

		if (result > 0)
		{
			PacketType packet_type;
			if (GetPacketType(buf, packet_type) && packet_type == MESSAGE)
			{
				MessagePacket* msg_packet = (MessagePacket*)buf;
				printf("%s\n", msg_packet->get_message());
			}
		}
		else if(IsWSAEWOULDBLOCK(result))
		{
			//It would be blocked, didn't recv packet
			continue;
		}
		else
		{
			ErrorHandling("NonBlockUdpClient::Recvfrom : recvfrom failed");
		}
	}
}
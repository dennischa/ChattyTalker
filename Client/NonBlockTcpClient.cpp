#include "Client.h"

NonBlockTcpClient::NonBlockTcpClient(SOCKADDR_IN serv_addr)
{
	clnt_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("BlockTcpClient : Invalid clnt socket", &clnt_socket_);
	}

	serv_addr_ = serv_addr;

	SetNonBlockMode(clnt_socket_);
}

bool NonBlockTcpClient::Connect()
{
	int result = connect(clnt_socket_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_));

	if (IsWSAEWOULDBLOCK(result))
	{
		FD_SET set;
		FD_ZERO(&set);
		FD_SET(clnt_socket_, &set);
		timeval time;
		time.tv_sec = 1;

		while (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			int s_result = select(0, nullptr, &set, nullptr, &time);

			if (s_result > 0)
			{
				return true;
			}
		}

		ErrorHandling("NonBlockTcpClient::Connect failed");
	}
	else if (result == SOCKET_ERROR)
	{
		ErrorHandling("NonBlockTcpClient::Connect failed");
	}

	return true;
}

void NonBlockTcpClient::Chat()
{
	char msg[MAX_MESSAGE_SIZE];
	bool on_chat = true;

	std::thread recv(&NonBlockTcpClient::Recv, this, std::ref(on_chat));

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

		while (true)
		{
			int result = send(clnt_socket_, (char*)& msg_packet, sizeof(msg_packet), 0);

			if (IsWSAEWOULDBLOCK(result))
			{
				continue;
			}
			else if (result > 0)
			{
				break;
			}
			else if( result == SOCKET_ERROR)
			{
				ErrorHandling(" NonBlockTcpClient::Chat : send failed");
			}
		}
	}

	closesocket(clnt_socket_);
	on_chat = false;

	if (recv.joinable())
	{
		recv.join();
	}
}

void NonBlockTcpClient::Recv(bool& on_chat)
{
	char buf[MAX_PACKET_SIZE];

	while (on_chat)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int result = recv(clnt_socket_, buf, MAX_PACKET_SIZE, 0);

		if (result > 0)
		{
			PacketType packet_type;
			if (GetPacketType(buf, packet_type) == MESSAGE)
			{
				MessagePacket* msg_packet = (MessagePacket*)buf;
				printf("%s\n", msg_packet->get_message());
			}
		}
		else if (IsWSAEWOULDBLOCK(result))
		{
			//It would be blocked, didn't recv packet
			continue;
		}
		else if (result == 0)
		{
			//close connection
			on_chat = false;
			break;
		}
		else
		{
			ErrorHandling("NonBlockTcpClient::Recv : recv failed");
		}
	}
}
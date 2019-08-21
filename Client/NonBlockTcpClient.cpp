#include "Client.h"

NonBlockTcpClient::NonBlockTcpClient(SOCKADDR_IN serv_addr): TcpClient(serv_addr)
{
	SetNonBlockMode(clnt_socket_);
}

bool NonBlockTcpClient::Connect()
{
	int result = connect(clnt_socket_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_));
	
	if (IsWSAEWOULDBLOCK(result))
	{
		while (true)
		{
			int s_result = Select();
			
			if (s_result > 0)
				return true;
			else if (s_result == 0)
				continue;
			else
				ErrorHandling("NonBlockTcpClient::Connect : failed Select");
		}
	}
	else if (result == SOCKET_ERROR)
	{
		ErrorHandling("NonBlockTcpClient::Connect() failed");
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

	on_chat = false;

	if (recv.joinable())
	{
		recv.join();
	}

	closesocket(clnt_socket_);
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

int NonBlockTcpClient::Select()
{
	FD_SET set;
	FD_ZERO(&set);
	FD_SET(clnt_socket_, &set);
	timeval time;
	time.tv_sec = 1;

	return select(0, nullptr, &set, nullptr, &time);
}
#include "Client.h"

OverlappedClient::OverlappedClient(const SOCKADDR_IN serv_addr)
{
	clnt_socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("OverlappedClient::OverlappedClient : Invalid clnt_socket_", &clnt_socket_);
	}

	serv_addr_ = serv_addr;

	if (InitSocketInfo(&send_sock_info_, clnt_socket_) == FALSE)
	{
		ErrorHandling("OverlappedClient::OverlappedClient : faile InitSocketInfo send_sock_info_", &clnt_socket_);
	}

	if (InitSocketInfo(&recv_sock_info_, clnt_socket_) == FALSE)
	{
		ErrorHandling("OverlappedClient::OverlappedClient : faile InitSocketInfo recv_sock_info_", &clnt_socket_);
	}
}

bool OverlappedClient::Connect()
{
	if (connect(clnt_socket_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("OverlappedClient::Connect : faile connect", &clnt_socket_);
		return false;
	}

	return true;
}

void OverlappedClient::Chat()
{
	char msg[MAX_MESSAGE_SIZE];
	bool on_chat = true;

	std::thread recv(&OverlappedClient::Recv, this, std::ref(on_chat));

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

		InitSocketInfo(&send_sock_info_, clnt_socket_);
		memcpy(send_sock_info_.buf, &msg_packet, sizeof(msg_packet));

		int result = WSASend(clnt_socket_, &send_sock_info_.wsabuf, 1, &send_sock_info_.bytes, send_sock_info_.flag, &send_sock_info_.ovelapped, NULL);

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				DWORD result = WSAWaitForMultipleEvents(1, &send_sock_info_.ovelapped.hEvent, FALSE, SEND_TIMEOUT, FALSE);
				
				if (result == WSA_WAIT_TIMEOUT)
				{
					printf("OverlappedClient::Chat() : WSASend TIMEOUT");
				}
			}
			else
			{
				ErrorHandling("OverlappedClient::Chat : failed WSASend");
			}
		}
	}

	on_chat = false;
	WSASetEvent(recv_sock_info_.ovelapped.hEvent);

	if (recv.joinable())
		recv.join();

	closesocket(clnt_socket_);
}

void OverlappedClient::Recv(bool& on_chat)
{
	while (on_chat)
	{
		if (InitSocketInfo(&recv_sock_info_, clnt_socket_))
		{
			int result = WSARecv(clnt_socket_, &recv_sock_info_.wsabuf, 1, &recv_sock_info_.bytes, &recv_sock_info_.flag, &recv_sock_info_.ovelapped, NULL);

			if (result == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
					ErrorHandling("OverlappedClient::Recv : failed WSARecv");

				DWORD wait = WSAWaitForMultipleEvents(1, &recv_sock_info_.ovelapped.hEvent, FALSE, WSA_INFINITE, FALSE);
				
				if (on_chat == FALSE)
					return;
				
				DWORD transfered, flag;
				
				bool get_result = WSAGetOverlappedResult(clnt_socket_, &recv_sock_info_.ovelapped, &transfered, FALSE, &flag);
				if (get_result == FALSE)
				{
					ErrorHandling("OverlappedClient::Recv : failed WSAGetOverlappedResult");
				}

				//ChatPacket* chat_packet = (ChatPacket * )recv_sock_info_.buf;
				PacketType type;
				GetPacketType(recv_sock_info_.buf, type);

				if (type != MESSAGE)
				{
					ErrorHandling("OverlappedClient::Recv : Wrong packet type");
				}

				MessagePacket* msg_packet = (MessagePacket*)recv_sock_info_.buf;
				printf("%s\n", msg_packet->get_message());
			}
		}
		else
		{
			ErrorHandling("OverlappedClient::Recv : failed InitSocketInfo");
		}
	}
}
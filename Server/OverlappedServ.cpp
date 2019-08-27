#include "Server.h"

using namespace ChattyTalker;

OverlappedServ::OverlappedServ()
{
	serv_sock_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("OverlappedServ::OverlappedServ(): Invalid serv_socket", &serv_sock_);
	}

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(OVERLAPPED_PORT);
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("OverlappedServ::OverlappedServ : Failed binding serv_sock" , &serv_sock_);
	}

	memset(sock_infos_, 0, sizeof(sock_infos_));
	memset(events_, 0, sizeof(events_));

	num_changed_event_ = WSACreateEvent();
	if (num_changed_event_ == WSA_INVALID_EVENT)
	{
		ErrorHandling("OverlappedServ::OverlappedServ : Invalid num_changed_event");
	}

	sock_num_ = 0;
	events_[sock_num_++] = num_changed_event_;
}

void OverlappedServ::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("OverlappedServ::Run : Listening serv_socket failed", &serv_sock_);
	}

	std::thread chat(&OverlappedServ::Chat, this);

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);

		accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);

		if (accp_sock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAECONNRESET)
				break;

			ErrorHandling("OverlappedServ::Run : Invalid accp_sock");
		}

		SocketInfo* sock_info_ptr = AddSocketInfo(accp_sock);

		if (sock_info_ptr == nullptr)
		{
			ErrorHandling("OverlappedServ::Run : Failed AddSocketInfo", &accp_sock);
		}

		int result = WSARecv(sock_info_ptr->socket, &(sock_info_ptr->wsabuf), 1, &(sock_info_ptr->bytes), &(sock_info_ptr->flag), &(sock_info_ptr->ovelapped), NULL);

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
				ErrorHandling("OverlappedServ::Run : failed WSARecv");
		}

		WSASetEvent(num_changed_event_);
	}

	if (chat.joinable())
		chat.join();
}

void OverlappedServ::Chat()
{
	while (serv_state_ == RUNNING)
	{
		DWORD wait_result = WSAWaitForMultipleEvents(sock_num_, events_, FALSE, WSA_INFINITE, true);
		
		int index = wait_result - WSA_WAIT_EVENT_0;

		if (index == 0)
		{
			WSAResetEvent(num_changed_event_);
			continue;
		}
		else if (index >= WSA_MAXIMUM_WAIT_EVENTS)
		{
			ErrorHandling("OverlappedServ::Chat : Wrong Index");
		}

		SocketInfo* ptr = &sock_infos_[index];

		DWORD transfered, flag;
		bool ov_result = WSAGetOverlappedResult(ptr->socket, &(ptr->ovelapped), &transfered, FALSE, &flag);

		if (ov_result == false)
		{
			if (WSAGetLastError() == WSAECONNRESET) //lose connection
			{
				RemoveSocketInfo(index);
				continue;
			}
			else
				ErrorHandling("OverlappedServ::Chat : Failed WSAGetOverlappedResult");
		}

		if (transfered == 0) //lose connection
		{
			RemoveSocketInfo(index);
			continue;
		}
			
		Send(index, ptr->buf, sizeof(ptr->buf));
		
		InitSocketInfo(ptr, ptr->socket);
		events_[index] = ptr->ovelapped.hEvent;
		
		WSARecv(ptr->socket, &ptr->wsabuf, 1, &ptr->bytes, &ptr->flag, &ptr->ovelapped, NULL);
	}  
}

SocketInfo* OverlappedServ::AddSocketInfo(SOCKET socket)
{
	mtx_.lock();

	if (sock_num_ >= WSA_MAXIMUM_WAIT_EVENTS)
	{
		mtx_.unlock();
		return nullptr;
	}

	SocketInfo* ptr = &sock_infos_[sock_num_];

	if (InitSocketInfo(ptr, socket) == false)
	{
		ErrorHandling("OverlappedServ::AddSocketInfo : Failed InitSocketInfo");
	}
	
	events_[sock_num_] = ptr->ovelapped.hEvent;
	sock_num_++;
	
	mtx_.unlock();

	return ptr;
}

void OverlappedServ::RemoveSocketInfo(int index)
{
	mtx_.lock();

	WSACloseEvent(events_[index]);
	closesocket(sock_infos_[index].socket);

	for (int i = index; i < sock_num_ - 1; i++)
	{
		sock_infos_[i] = sock_infos_[i + 1];
		events_[i] = events_[i + 1];
	}

	sock_num_--;

	mtx_.unlock();
}

void OverlappedServ::Send(int index, char* buf, int size)
{
	const int num = sock_num_;
	SocketInfo sock_info;

	for (int i = 1; i < num; i++)
	{
		if (index == i)
			continue;

		if (InitSocketInfo(&sock_info, sock_infos_[i].socket))
		{
			memcpy(sock_info.buf, buf, size);

			int send_result = WSASend(sock_info.socket, &sock_info.wsabuf, 1, &sock_info.bytes, sock_info.flag, &sock_info.ovelapped, NULL);

			if (send_result == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSA_IO_PENDING)
				{
					DWORD result = WSAWaitForMultipleEvents(1, &sock_info.ovelapped.hEvent, FALSE, SEND_TIMEOUT, FALSE);

					if (result == WSA_WAIT_TIMEOUT)
					{
						printf("OverlappedClient::Chat() : WSASend TIMEOUT");
					}
				}
				else
				{
					ErrorHandling("OverlappedServ::Send : failed Send");
				}
			}

		}
	}
}

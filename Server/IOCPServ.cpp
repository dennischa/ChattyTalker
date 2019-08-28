#include "Server.h"

using namespace ChattyTalker;

IOCPServ::IOCPServ()
{
	serv_sock_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("IOCPServ::IOCPServ : Invalid serv_socket", &serv_sock_);
	}

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(IOCP_PORT);
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("IOCPServ::IOCPServ : Failed binding serv_sock", &serv_sock_);
	}

	completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (completion_port_ == NULL)
		ErrorHandling("IOCPServ::IOCPServ Failed CreateIoCompletionPort");
}

void IOCPServ::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("IOCPServ::Run : Listening serv_socket failed", &serv_sock_);
	}

	std::thread chat(&IOCPServ::Chat, this);

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

		HANDLE handle = CreateIoCompletionPort((HANDLE)accp_sock, completion_port_, accp_sock, 0); //¿¬°á
		if (handle != completion_port_)
		{
			ErrorHandling("IOCPServ::Run : failed associate CreateIoCompletionPort");
		}

		SocketInfo* sock_info_ptr = new SocketInfo;
		InitSocketInfo(sock_info_ptr, accp_sock);

		if (sock_info_ptr == nullptr)
		{
			ErrorHandling("IOCPServ::Run : Failed InitSocketInfo", &accp_sock);
		}

		sock_infos_[accp_sock] = (sock_info_ptr);

		int result = WSARecv(sock_info_ptr->socket, &(sock_info_ptr->wsabuf), 1, &(sock_info_ptr->bytes), &(sock_info_ptr->flag), &(sock_info_ptr->ovelapped), NULL);

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
				ErrorHandling("OverlappedServ::Run : failed WSARecv");
		}
	}

	if (chat.joinable())
		chat.join();
}

void IOCPServ::Chat()
{
	while (serv_state_ == RUNNING)
	{
		DWORD transfered;
		OVERLAPPED ovelapped;
		SOCKET clnt_sock;

		bool result = GetQueuedCompletionStatus(completion_port_, &transfered, &clnt_sock, (LPOVERLAPPED*)&ovelapped, WSA_INFINITE);
		
		std::map<SOCKET, SocketInfo*>::iterator it = sock_infos_.find(clnt_sock);
		if (it == sock_infos_.end())
		{
			//Errr
		}

		if (result == false || transfered == 0)
		{
			delete it->second;
			sock_infos_.erase(it);
			closesocket(clnt_sock);
		}

		SocketInfo* ptr= sock_infos_[clnt_sock];
		
		Send(clnt_sock, ptr->buf);

		InitSocketInfo(ptr, clnt_sock);
		WSARecv(clnt_sock, &ptr->wsabuf, 1, &ptr->bytes, &ptr->flag, &ptr->ovelapped, NULL);
	}
}

IOCPServ::~IOCPServ()
{
	CloseHandle(completion_port_);
}

void IOCPServ::Send(SOCKET sock, char* buf)
{
	std::map<SOCKET, SocketInfo*>::iterator it = sock_infos_.begin();

	for(; it != sock_infos_.end(); it++)
	{
		if (it->first == sock)
			continue;

		SocketInfo* ptr = it->second;
		send(it->first, buf, sizeof(buf), 0);
	}
}
#include "Server.h"

NonBlockTcpServ::NonBlockTcpServ()
{
	serv_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("NonBlockTcpServ: Invalid serv_sock", &serv_sock_);
	}

	memset(&serv_addr_, 0, sizeof(serv_addr_));

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(NBLOCK_TCP_PORT);
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpServ: Binding serv_sock failed", &serv_sock_);
	}

	SetNonBlockMode(serv_sock_);
}

void NonBlockTcpServ::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("NonBlockTcpServ: Listening serv_socket failed", &serv_sock_);
	}

	std::thread chat(&NonBlockTcpServ::Chat, this);

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);

		while (true)
		{
			accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);
			
			if (accp_sock == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
				else
				{
					ErrorHandling("NonBlockTcpServ::Run : Invalid accp_socket", &accp_sock);
				}
			}

			break;
		}

		clnt_socks_[accp_sock] = clnt_addr;

		printf("BlockTcpServ : Accept Client Connect : %s\n", toString(clnt_addr).c_str());
	}

	if (chat.joinable())
		chat.join();
}

void NonBlockTcpServ::Chat()
{
	char buf[MAX_PACKET_SIZE];
	std::map<SOCKET, SOCKADDR_IN>::iterator it;

	while (serv_state_ == RUNNING)
	{
		memset(buf, 0, MAX_PACKET_SIZE);
		
		int select_num = Select();

		if (select_num == 0)
		{
			//timeout
			continue;
		}
		else if(select_num < 0)
		{
			ErrorHandling(" NonBlockTcpServ::Chat : select failed");
		}


		std::map<SOCKET, SOCKADDR_IN>::iterator it = clnt_socks_.begin();
		for (; it != clnt_socks_.end(); it++)
		{
			SOCKET socket = it->first;
			memset(buf, 0, MAX_PACKET_SIZE);

			int result = recv(socket, buf, MAX_PACKET_SIZE, 0);

			if(result > 0)
			{
				select_num--;
				Send(socket, buf);
			}
			else if(result == 0)
			{
				select_num--;
				clnt_socks_.erase(it);
			}
			else if (IsWSAEWOULDBLOCK(result))
			{
				continue;
			}
			else
			{
				ErrorHandling("NonBlockTcpServ::Chat : recv failed");
			}

			if (select_num == 0)
				break;
		}
	}
}

int NonBlockTcpServ::Select()
{
	FD_SET set;
	FD_ZERO(&set);
	timeval time;
	time.tv_sec = 1;

	std::map<SOCKET, SOCKADDR_IN>::iterator it = clnt_socks_.begin();
	

	for (; it != clnt_socks_.end(); it++)
	{
		FD_SET(it->first, &set);
	}

	return select(0, &set, nullptr, nullptr, &time);

}

void NonBlockTcpServ::Send(SOCKET& socket, char* buf)
{
	std::map<SOCKET, SOCKADDR_IN>::iterator it = clnt_socks_.begin();

	for (; it != clnt_socks_.end(); it++)
	{
		SOCKET clnt_sock = it->first;
		if (clnt_sock != socket)
		{
			while (true)
			{
				int result = send(clnt_sock, buf, sizeof(buf), 0);

				if (IsWSAEWOULDBLOCK(result))
				{
					continue;
				}

				break;
			}
			
		}
	}
}
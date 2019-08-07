#include "Server.h"

BlockTcpServ::BlockTcpServ()
{
	serv_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("BlockTcpServ: Invalid serv_sock", &serv_sock_);
	}

	memset(&serv_addr_, 0, sizeof(serv_addr_));

	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_port = htons(BLOCK_TCP_PORT);
	inet_pton(AF_INET, Serv_IPv4_ADDR, &serv_addr_.sin_addr.S_un.S_addr);

	if (bind(serv_sock_, (SOCKADDR*)& serv_addr_, sizeof(serv_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpServ: Binding serv_sock failed", &serv_sock_);
	}
}

void BlockTcpServ::Run()
{
	serv_state_ = RUNNING;

	if (listen(serv_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("BlockTcpServ: Listening serv_socket failed", &serv_sock_);
	}

	while (serv_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;
		int addrlen = sizeof(clnt_addr);

		accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);
		if (accp_sock == INVALID_SOCKET)
		{
			ErrorHandling("BlockTcpServ: Invalid accp_socket", &accp_sock);
		}

		clnt_socks_[accp_sock] = clnt_addr;

		printf("Lobby : Accept Client Connect : %s\n", toString(clnt_addr).c_str());

		//클라이언트랑 대화하는 쓰레드 생성
		std::thread lobby_chat([&]() {Chat(accp_sock); });
		lobby_chat.detach();
	}
}

void BlockTcpServ::Chat(SOCKET socket)
{
	char buf[MAX_PACKET_SIZE];

	while (serv_state_ == RUNNING)
	{
		memset(buf, 0, MAX_PACKET_SIZE);

		int r = recv(socket, buf, MAX_PACKET_SIZE, 0);

		
	}
}
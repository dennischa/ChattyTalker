#include "Lobby.h"

Lobby::Lobby()
{
	lobby_state_ = STOPPED;
}

Lobby::~Lobby()
{
	STOP();
}

void Lobby::RUN()
{
	lobby_state_ = RUNNING;
	lobby_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (lobby_sock_ == INVALID_SOCKET)
	{
		ErrorHandling("Lobby: Invalid lobby socket", &lobby_sock_);
	}

	memset(&lobby_addr_, 0, sizeof(lobby_addr_));

	lobby_addr_.sin_family = AF_INET;
	lobby_addr_.sin_port = htons(LOBBY_PORT);
	lobby_addr_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(lobby_sock_, (SOCKADDR*)& lobby_addr_, sizeof(lobby_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Binding lobby socket failed", &lobby_sock_);
	}

	if (listen(lobby_sock_, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorHandling("Lobby: Listening lobby socket failed", &lobby_sock_);
	}

	while (lobby_state_ == RUNNING)
	{
		SOCKET accp_sock;
		SOCKADDR_IN clnt_addr;

		accp_sock = accept(lobby_sock_, (SOCKADDR*)& clnt_addr, nullptr);
		if (accp_sock == INVALID_SOCKET)
		{
			ErrorHandling("Lobby: Invalid accp_socket", &accp_sock);
		}

		clnt_socks_[accp_sock] = clnt_addr;
		char addr_in[20];
		inet_ntop(AF_INET, &clnt_addr.sin_addr.S_un.S_addr, addr_in, sizeof(addr_in));
		printf("Connected %s\n", addr_in);

		//클라이언트랑 대화하는 쓰레드 생성
	}

	lobby_state_ = STOPPED;
}

void Lobby::STOP()
{
	if (lobby_state_ == RUNNING)
	{
		closesocket(lobby_sock_);

		std::map<SOCKET, SOCKADDR_IN>::iterator index = clnt_socks_.begin();
		for (; index != clnt_socks_.end(); index++)
		{
			closesocket((*index).first);
		}

		lobby_state_ = STOPPED;
	}
}


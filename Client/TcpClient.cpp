#include "Client.h"

TcpClient::TcpClient()
{
	clnt_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("TcpClient::TcpClient() : Invalid socket", &clnt_socket_);
	}
}
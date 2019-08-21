#include "Client.h"

UdpClient::UdpClient(SOCKADDR_IN serv_addr)
{
	clnt_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (clnt_socket_ == INVALID_SOCKET)
	{
		ErrorHandling("UdpClient::UdpClient : Invalid clnt_sock_ ", &clnt_socket_);
	}

	serv_addr_ = serv_addr;

	memset(&clnt_addr_, 0, sizeof(clnt_addr_));
	clnt_addr_.sin_family = AF_INET;
	clnt_addr_.sin_port = htons(0);
	inet_pton(AF_INET, Clnt_IPv4_ADDR, &clnt_addr_.sin_addr.S_un.S_addr);

	if (bind(clnt_socket_, (SOCKADDR*)& clnt_addr_, sizeof(clnt_addr_)) == SOCKET_ERROR)
	{
		ErrorHandling("UdpClient::UdpClient : binding socket failed", &clnt_socket_);
	}
}
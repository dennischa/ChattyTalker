#include "Client.h"
using namespace std;

Client::Client()
{
	memset(&serv_addr_, 0, sizeof(serv_addr_));
	serv_addr_.sin_family = AF_INET;

	memset(&clnt_addr_, 0, sizeof(clnt_addr_));
	clnt_addr_.sin_family = AF_INET;
	clnt_addr_.sin_port = htons(0);
	inet_pton(AF_INET, Clnt_IPv4_ADDR, &clnt_addr_.sin_addr.S_un.S_addr);
}

Client::~Client()
{
	closesocket(clnt_socket_);
}

SOCKADDR_IN Client::get_clnt_addr()
{
	SOCKADDR_IN addr;
	int len = sizeof(addr);
	int sockname = getsockname(clnt_socket_, (SOCKADDR*)& addr, &len);
	
	if (sockname != 0)
	{
		ErrorHandling("Client::get_clnt_addr() : getsockname failed");
	}

	return addr;
}

void ShowGuide()
{
	std::string in_line;
	std::ifstream in("guide.txt");

	while (std::getline(in, in_line))
	{
		std::cout << in_line << std::endl;
	}

	in.close();
}
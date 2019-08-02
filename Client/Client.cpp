#include "Client.h"
using namespace std;

Client::Client()
{
	memset(&serv_addr_, 0, sizeof(serv_addr_));
	serv_addr_.sin_family = AF_INET;
}

Client::~Client()
{
	closesocket(clnt_socket_);
}
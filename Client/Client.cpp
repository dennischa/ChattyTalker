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

SOCKADDR_IN UdpClient::get_clnt_addr()
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

void PrintMsg(char* buf)
{
	PacketType packet_type;
	if (GetPacketType(buf, packet_type) != MESSAGE)
	{
		printf("Error : ! Not Message Packet\n");
		return;
	}
	
	printf("Recv Message : %s \n", ((MessagePacket*)buf)->get_message());
}
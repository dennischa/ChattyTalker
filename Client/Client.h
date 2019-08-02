#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Common.h"
#include "ChatPacket.h"

class Client
{
public:
	Client();
	~Client();
	virtual bool Connect() { return true; }
	virtual void Chat() = 0;
	SOCKET get_clnt_socket() { return clnt_socket_; }
protected:
	SOCKET clnt_socket_;
	SOCKADDR_IN serv_addr_;
};

class LobbyClient : public Client
{
public:
	LobbyClient();
	virtual bool Connect();
	virtual void Chat();
};

class BlockUdpClient : public Client
{
public:
	BlockUdpClient(SOCKADDR_IN serv_addr);
	virtual void Chat();
};


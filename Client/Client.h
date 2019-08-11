#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include "Common.h"
#include "ChatPacket.h"

#define Clnt_IPv4_ADDR "127.0.0.1"

class Client
{
public:
	Client();
	~Client();
	virtual bool Connect() { return true; }
	virtual void Chat() = 0;
	SOCKET get_clnt_socket() { return clnt_socket_; }
	SOCKADDR_IN get_clnt_addr();
protected:
	SOCKET clnt_socket_;
	SOCKADDR_IN serv_addr_;
	SOCKADDR_IN clnt_addr_;
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
	void Recvfrom(bool& on_chat);
};

class BlockTcpClient : public Client
{
public:
	BlockTcpClient(SOCKADDR_IN serv_addr);
	virtual bool Connect();
	virtual void Chat();
	void Recv(bool& on_chat);
};

class NonBlockUdpClient : public Client
{
public:
	NonBlockUdpClient(SOCKADDR_IN serv_addr);
	virtual void Chat();
	void Recvfrom(bool& on_chat);
};

class NonBlockTcpClient : public Client
{
public:
	NonBlockTcpClient(SOCKADDR_IN serv_addr);
	virtual bool Connect();
	virtual void Chat();
	void Recv(bool& on_chat);
};

void ShowGuide();
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
	virtual void Chat() = 0;
	SOCKET get_clnt_socket() { return clnt_socket_; }

protected:
	SOCKET clnt_socket_;
	SOCKADDR_IN serv_addr_;
};

class TcpClient : public Client
{
public:
	TcpClient();
	TcpClient(const SOCKADDR_IN serv_addr) :TcpClient()
	{
		serv_addr_ = serv_addr;
	}
	virtual bool Connect() = 0;

protected:
	virtual void Recv(bool& on_chat) {};

};

class UdpClient : public Client
{
public:
	UdpClient(const SOCKADDR_IN serv_addr);
	SOCKADDR_IN get_clnt_addr();

private:
	virtual void Recvfrom(bool& on_chat) = 0;
	SOCKADDR_IN clnt_addr_;
};

class LobbyClient : public TcpClient
{
public:
	LobbyClient();
	virtual bool Connect();
	virtual void Chat();
	Client* MakeClient(RoomType type, SOCKADDR_IN addr);
};

class BlockUdpClient : public UdpClient
{
public:
	BlockUdpClient(const SOCKADDR_IN serv_addr) : UdpClient(serv_addr) {}
	virtual void Chat();
	virtual void Recvfrom(bool& on_chat);
};

class BlockTcpClient : public TcpClient
{
public:
	BlockTcpClient(SOCKADDR_IN serv_addr) : TcpClient(serv_addr) {}
	virtual bool Connect();
	virtual void Chat();
	void Recv(bool& on_chat);
};

class NonBlockUdpClient : public UdpClient
{
public:
	NonBlockUdpClient(const SOCKADDR_IN serv_addr);
	virtual void Chat();
	virtual void Recvfrom(bool& on_chat);
};

class NonBlockTcpClient : public TcpClient
{
public:
	NonBlockTcpClient(SOCKADDR_IN serv_addr);
	virtual bool Connect();
	virtual void Chat();
	virtual void Recv(bool& on_chat);
	int Select();
};

class OverlappedClient : public Client
{
public:
	OverlappedClient(const SOCKADDR_IN serv_addr);
	bool Connect();
	virtual void Chat();

private:
	void Recv(bool &on_chat);
	SocketInfo send_sock_info_;
	SocketInfo recv_sock_info_;
};

void ShowGuide();
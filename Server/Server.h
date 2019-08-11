#pragma once
#include <map>
#include <vector>
#include <thread>
#include <Common.h>
#include <queue>
#include <mutex>
#include "ChatPacket.h"

#define BLOCK_UDP_PORT 4100
#define BLOCK_TCP_PORT 4200
#define NBLOCK_UDP_PORT 4300
#define NBLOCK_TCP_PORT 4400


enum ServerState
{
	STOPPED = 0,
	RUNNING = 1
	
};

class Server
{
public:
	Server();
	~Server();
	virtual void Run() {}
	virtual void Stop();
	virtual void Chat(SOCKET socket) {};
	SOCKADDR_IN get_serv_addr() { return serv_addr_; }

protected:
	SOCKET serv_sock_;
	SOCKADDR_IN serv_addr_;
	std::map<SOCKET, SOCKADDR_IN> clnt_socks_;
	ServerState serv_state_;
	std::vector<std::thread> threads;
};

class Lobby : public Server
{
public:
	Lobby();
	virtual void Run();
	virtual void Chat(SOCKET socket);
	Server* FindChatroom(RoomType room_type);

private:
	std::map<RoomType, Server*> chat_rooms_;
};

class BlockUdpServ : public Server
{
public:
	BlockUdpServ();
	virtual void Run();
	 bool Join(SOCKADDR_IN addr);
	bool Leave(SOCKADDR_IN addr);

private:
	std::vector<SOCKADDR_IN> clnt_addrs_;
};

class BlockTcpServ : public Server
{
public:
	BlockTcpServ();
	virtual void Run();
	virtual void Chat(SOCKET socket);
};

class NonBlockUdpServ : public BlockUdpServ
{
public:
	NonBlockUdpServ();
	virtual void Run();
	bool Join(SOCKADDR_IN addr);
	bool Leave(SOCKADDR_IN addr);

private:
	std::vector<SOCKADDR_IN> clnt_addrs_;
};

class NonBlockTcpServ : public Server
{
public:
	NonBlockTcpServ();
	virtual void Run();
	void Chat();
	int Select();
	void Send(SOCKET& socket, char* buf);
};

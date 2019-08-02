#pragma once
#include <map>
#include <vector>
#include <thread>
#include <Common.h>
#include "ChatPacket.h"

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
};

class Lobby : public Server
{
public:
	Lobby();
	virtual void Run();
	virtual void Chat(SOCKET socket);

private:
	std::map<RoomType, Server*> chat_rooms_;
};

class BlockUdpServ : public Server
{
public:
	BlockUdpServ();
	virtual void Run();
	bool Join(SOCKADDR_IN addr);

private:
	std::vector<SOCKADDR_IN> clnt_addrs_;
};


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
#define OVERLAPPED_PORT 4500


enum ServerState
{
	STOPPED = 0,
	RUNNING = 1
};

namespace ChattyTalker
{
	class Server
	{
	public:
		Server();
		~Server();
		virtual void Stop();
		virtual void Run() = 0;
		SOCKADDR_IN get_serv_addr() { return serv_addr_; }

	protected:
		SOCKET serv_sock_;
		SOCKADDR_IN serv_addr_;
		ServerState serv_state_;
		std::vector<std::thread> threads;
	};

	class TcpServer : public Server
	{
	public:
		TcpServer();
		virtual void Stop();
		virtual void Chat(SOCKET socket = NULL) = 0;

	protected:
		std::map<SOCKET, SOCKADDR_IN> clnt_socks_;
	};

	class UdpServer : public Server
	{
	public:
		UdpServer();
		bool Join(SOCKADDR_IN addr);
		bool Leave(SOCKADDR_IN addr);

	protected:
		std::vector<SOCKADDR_IN> clnt_addrs_;
	};

	class Lobby : public TcpServer
	{
	public:
		Lobby();
		~Lobby();
		virtual void Run();
		virtual void Chat(SOCKET socket);
		ChattyTalker::Server* FindChatroom(RoomType room_type);

	private:
		std::map<RoomType, ChattyTalker::Server*> chat_rooms_;
	};

	class BlockUdpServ : public UdpServer
	{
	public:
		BlockUdpServ();
		virtual void Run();
	};

	class BlockTcpServ : public TcpServer
	{
	public:
		BlockTcpServ();
		virtual void Run();
		virtual void Chat(SOCKET socket);
	};

	class NonBlockUdpServ : public UdpServer
	{
	public:
		NonBlockUdpServ();
		virtual void Run();
	};

	class NonBlockTcpServ : public TcpServer
	{
	public:
		NonBlockTcpServ();
		virtual void Run();
		virtual void Chat(SOCKET socke);

	private:
		int Select();
		void Send(SOCKET& socket, char* buf);
	};
	
	class OverlappedServ : public Server
	{
	public:
		OverlappedServ();
		virtual void Run();

	private:
		void Chat();
		SocketInfo* AddSocketInfo(SOCKET socket);
		void RemoveSocketInfo(int index);
		void Send(int index, char* buf, int size);

		SocketInfo sock_infos_[WSA_MAXIMUM_WAIT_EVENTS];
		WSAEVENT events_[WSA_MAXIMUM_WAIT_EVENTS];
		int sock_num_;
		WSAEVENT num_changed_event_;
		std::mutex mtx_;
	};
}
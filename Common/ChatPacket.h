#pragma once
#include "Common.h"

#define MAX_PACKET_SIZE 320
#define MAX_MESSAGE_SIZE 300

enum PacketType
{
	DEFAULT = 0,
	MESSAGE = 1,
	JOIN = 2,
	LEAVE = 3,
	HOST_INFO =4,
	PacketTypeCount
};

enum RoomType
{
	BLOCK_UDP = 1,
	BLOCK_TCP = 2,
	NONBLOCK_UDP = 3,
	NONBLOCK_TCP =4,
	EPOLL = 5,
	OVERLLAPED = 6,
	IOCP =7
};

class ChatPacket
{
public:
	ChatPacket(PacketType type = DEFAULT) : packet_type_(type) {}
	PacketType get_packet_type() { return packet_type_; }
private:
	PacketType packet_type_;
};

class MessagePacket : public ChatPacket
{
public:
	MessagePacket(const char* c);
	char* get_message() { return message_; }
private:
	char message_[MAX_MESSAGE_SIZE];
};

class JoinPacket : public ChatPacket
{
public:
	JoinPacket(RoomType num) : ChatPacket(JOIN), room_type_(num) {}
	RoomType get_room_type() { return room_type_; }
private:
	RoomType room_type_;
};

class HostInfoPacket : public ChatPacket
{
public:
	HostInfoPacket(const SOCKADDR_IN host_addr, const RoomType room_type) :ChatPacket(HOST_INFO), host_addr_(host_addr), room_type_(room_type) {}
	SOCKADDR_IN get_host_addr() { return host_addr_; }
	RoomType get_room_type() { return room_type_; }
private:
	SOCKADDR_IN host_addr_;
	RoomType room_type_;
};

class LeavePacket : public ChatPacket
{
public:
	LeavePacket(RoomType romm_type, SOCKADDR_IN addr) :ChatPacket(LEAVE), room_type_(romm_type), addr_(addr) {}
	RoomType get_room_type() {	return room_type_; }
	SOCKADDR_IN get_addr() { return addr_; }
private:
	RoomType room_type_;
	SOCKADDR_IN addr_;

};

bool GetPacketType(char* buf, PacketType& packet_type);
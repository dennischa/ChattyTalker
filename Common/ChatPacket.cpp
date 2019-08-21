#include "ChatPacket.h"

MessagePacket::MessagePacket(const char* c) : ChatPacket(MESSAGE)
{
	strcpy_s(message_, c);
}

bool GetPacketType(char* buf, PacketType& packet_type)
{
	ChatPacket* chat_packet = (ChatPacket*)buf;
	PacketType type = chat_packet->get_packet_type();

	if (type < 0 || type >= PacketTypeCount)
		return false;

	packet_type = type;
	return true;
}

bool InitSocketInfo(SocketInfo* ptr, SOCKET socket)
{
	if (ptr == nullptr)
		return false;

	if(ptr->socket != socket)
		ptr->socket = socket;

	memset(&(ptr->ovelapped), 0, sizeof(OVERLAPPED));
	ptr->ovelapped.hEvent = WSACreateEvent();
	if (ptr->ovelapped.hEvent == WSA_INVALID_EVENT)
	{
		return false;
	}

	memset(ptr->buf, 0, MAX_PACKET_SIZE);
	ptr->wsabuf.len = MAX_PACKET_SIZE;
	ptr->wsabuf.buf = ptr->buf;
	
	ptr->bytes = 0;
	ptr->flag = 0;

	return true;
}
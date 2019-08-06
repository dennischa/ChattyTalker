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
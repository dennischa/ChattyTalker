#include "ChatPacket.h"

MessagePacket::MessagePacket(char* c) : ChatPacket(MESSAGE)
{
	strcpy_s(message_, c);
}
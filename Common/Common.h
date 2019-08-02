#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define LOBBY_PORT 4000
#define BLOCK_UDP_PORT 4010
#define Serv_IPv4_ADDR "127.0.0.1"
#define MAX_MESSAGE_SIZE 300

void ErrorHandling(const char* message, SOCKET* socket = nullptr);
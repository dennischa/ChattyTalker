#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <functional>
#include <utility> 

#define Serv_IPv4_ADDR "127.0.0.1"
#define LOBBY_PORT 4000
#define SEND_TIMEOUT 1000

void ErrorHandling(const char* message, SOCKET* socket = nullptr);
std::string toString(SOCKADDR_IN& addr);
bool Equal(SOCKADDR_IN laddr, SOCKADDR_IN raddr);
bool IsWSAEWOULDBLOCK(const int result);
bool SetNonBlockMode(SOCKET& socket);
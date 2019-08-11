#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <functional>

#define Serv_IPv4_ADDR "127.0.0.1"
#define LOBBY_PORT 4000

void ErrorHandling(const char* message, SOCKET* socket = nullptr);
std::string toString(SOCKADDR_IN& addr);
bool Equal(SOCKADDR_IN laddr, SOCKADDR_IN raddr);
bool IsWSAEWOULDBLOCK(const int result);
bool SetNonBlockMode(SOCKET& socket);
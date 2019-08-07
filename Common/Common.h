#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

#define MAX_MESSAGE_SIZE 300

void ErrorHandling(const char* message, SOCKET* socket = nullptr);
std::string toString(SOCKADDR_IN& addr);
bool Equal(SOCKADDR_IN laddr, SOCKADDR_IN raddr);
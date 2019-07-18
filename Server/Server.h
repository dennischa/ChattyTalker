#pragma once
#include <stdio>
#include <WinSock2.h>

void ErrorHandling(const char* message, SOCKET* socket = nullptr);

enum STATE
{
	RUNNING,
	STOPPED
};
#pragma once
#include <stdio.h>
#include <WinSock2.h>

void ErrorHandling(const char* message, SOCKET* socket = nullptr);

enum State
{
	RUNNING,
	STOPPED
};
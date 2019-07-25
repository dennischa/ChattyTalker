#pragma once
enum ChatRoomType
{
	Block_UDP,
	Block_TCP,
	NonBlock_UDP,
	NonBlock_TCP,
	Overlapped_IO,
	Epoll,
	ICOP,
	P2P
};
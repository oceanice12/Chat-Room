#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#ifndef DEFAULT_BUFLEN
#define DEFAULT_BUFLEN 512
#endif


class User
{
public:
	User(std::string name);

	// Set the user's addr through the accept() function
	sockaddr* AcceptAddr() { return &addr; }

	const SOCKET GetSocket() const { return sock; }

	SOCKET& SetSocket() { return sock; }

	void Chat(std::vector<User*>& users) const;

	const std::string name;

private:
	SOCKET sock;
	sockaddr addr;
};
// Chat program

#include "chat.h"


User::User(std::string name) : name{ name }, sock{ INVALID_SOCKET }
{
	ZeroMemory(&addr, (int)sizeof(addr));
}

void User::Chat(std::vector<User*>& users) const
{
	// Data handling
	char emptybuf[DEFAULT_BUFLEN] = { '\0' };
	char recvbuf[DEFAULT_BUFLEN] = { '\0' };
	int iResult;
	int iSendResult;
	std::string message;

	do
	{
		message.clear();
		strcpy_s(recvbuf, emptybuf);
		iResult = recv(sock, recvbuf, DEFAULT_BUFLEN, NULL);

		if (iResult > 0)
		{
			message = recvbuf;
			std::cout << "\nBytes received: " << iResult << std::endl;
			message = name + ": " + message;
			message.resize(name.length() + 2 + iResult);
			std::cout << message << std::endl;

			// Send message to all other users
			for (const User *u : users)
			{
				std::cout << "Sending message to " << u->name << std::endl;
				iSendResult = send(u->GetSocket(), message.c_str(), message.length(), NULL);
				if (iSendResult == SOCKET_ERROR)
					std::cout << "Message failed to send..." << std::endl;
			}
		}
		else if (iResult == 0)
			std::cout << "Connection closing..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;

	} while (iResult > 0);

	closesocket(sock);
	std::cout << name << " has disconnected." << std::endl;
	return;
}
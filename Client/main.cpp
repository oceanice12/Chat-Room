// Client

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstdlib>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "28000"
#define DEFAULT_BUFLEN 512

std::vector<std::thread> threads;

void Receive(SOCKET& sock);
void Send(SOCKET& sock);

void Cleanup();

int main()
{
	// Init winsock
	int iResult;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "Error code: " << iResult << std::endl;
		std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// Set socket parameters
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	std::string serverAddr;
	std::cout << "Enter server IP: ";
	std::getline(std::cin, serverAddr);

	std::string port;
	std::cout << "Port (Leave blank to use the default port): ";
	std::getline(std::cin, port);
	if (port.empty())
		port = DEFAULT_PORT;
	std::cout << "Connecting on port " << port << "...\n\n" << std::endl;

	iResult = getaddrinfo(serverAddr.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create SOCKET for connecting to server
	SOCKET sock = INVALID_SOCKET;
	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (sock == INVALID_SOCKET) 
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server
	iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		std::cout << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return 1;
	}

	std::cout << "Connection successful!\nReady to chat!\n" << std::endl;

	// Chat

	// Thread for receiving data
	std::thread tIn{ Receive, std::ref(sock) };
	threads.push_back(std::move(tIn));

	Send(sock);


	// Thread for sending data
	//std::thread tOut{ Send, std::ref(clientSocket) };
	//threads.push_back(std::move(tOut));

	
}

// Receive data until the server closes the connection
void Receive(SOCKET& sock)
{
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	std::string message;

	do 
	{
		iResult = recv(sock, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			message = recvbuf;
			message.resize(iResult);
			std::cout << message << "\n" << std::endl;
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	} while (iResult > 0);

	return;
}

void Send(SOCKET& sock)
{
	int iResult;
	std::string sendbuf;

	do
	{
		std::getline(std::cin, sendbuf);
		iResult = send(sock, sendbuf.c_str(), (int)sendbuf.length(), NULL);

		if (iResult == SOCKET_ERROR)
			std::cout << "Message failed to send..." << std::endl;
	} while (iResult != SOCKET_ERROR);

	return;
}

void Cleanup()
{
	for (int i = 0, s = threads.size(); i < s; i++)
		threads[i].join();

	WSACleanup();
	return;
}
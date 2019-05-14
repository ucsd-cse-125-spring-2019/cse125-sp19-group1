#include "ClientNetwork.h"

#include <windows.h>
#pragma comment (lib, "User32.lib")

#include <vector>

#define ShowErrorPopup(...) {\
	size_t len = snprintf(NULL, 0, __VA_ARGS__ ) + 1;\
	std::vector<char> msg(len, 0);\
	snprintf(&msg[0], len, __VA_ARGS__ );\
	MessageBox(NULL, &msg[0], "ERROR", MB_APPLMODAL | MB_ICONERROR);\
}

ClientNetwork::ClientNetwork(void) 
{
	// create WSADATA object
	WSADATA wsaData;

	// socket
	ConnectSocket = INVALID_SOCKET;

	// holds address info for socket to connect to
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) 
    {
		ShowErrorPopup("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!
	//resolve server address and port 
	//iResult = getaddrinfo("128.54.70.16", DEFAULT_PORT, &hints, &result);
	//iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo(SERVER_IP_ADDRESS, DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		ShowErrorPopup("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) 
        {
			ShowErrorPopup("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			ShowErrorPopup("The server is down... did not connect");
		}
	}

	// no longer need address info for server
	freeaddrinfo(result);

	// if connection failed
	if (ConnectSocket == INVALID_SOCKET)
	{
		ShowErrorPopup("Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;

	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		ShowErrorPopup("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	//disable nagle
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

}

int ClientNetwork::receivePackets(char * recvbuf)
{
	iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	if (iResult == 0)
	{
		ShowErrorPopup("Connection closed\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	return iResult;
}
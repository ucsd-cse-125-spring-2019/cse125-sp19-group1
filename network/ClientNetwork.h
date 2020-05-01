#pragma once

// Networking libraries
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include "NetworkData.h"
#include <ws2tcpip.h>
#include <stdio.h> 

extern char serverIpAddress[64];

// server address info (modify desktop ip as needed)
#define DESKTOP_IP "128.54.70.15"
#define LOCAL_HOST "127.0.0.1"
#define SERVER_IP_ADDRESS serverIpAddress

// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT "27015"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class ClientNetwork
{
 
public:
    // for error checking function calls in Winsock library
    int iResult;
 
    // socket for client to connect to server
    SOCKET ConnectSocket;
 
    // ctor/dtor
    ClientNetwork(void);
    ~ClientNetwork();

	int receivePackets(char *);
};

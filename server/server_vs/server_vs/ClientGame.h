#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include <iostream>
#include <sstream>

class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);

	char network_data[MAX_PACKET_SIZE];

	void update();
};
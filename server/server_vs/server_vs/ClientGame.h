#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);
	void sendMovementPackets(int type);

	char network_data[MAX_PACKET_SIZE];

	void update();
};
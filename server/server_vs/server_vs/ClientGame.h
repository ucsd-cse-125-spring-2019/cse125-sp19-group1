#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include <iostream>
#include <sstream>
#include <string>

class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);
	void sendForwardPackets();

	char network_data[MAX_PACKET_SIZE];

	void update();
	void parseMovementData();

private:
	static std::string my_client_id;
};
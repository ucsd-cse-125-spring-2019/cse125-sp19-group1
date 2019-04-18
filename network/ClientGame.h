#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include "Player.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>


class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	std::map < std::string, std::vector<int> > clients2;


	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);
	void sendForwardPackets();

	char network_data[MAX_PACKET_SIZE];

	void update();
	void parseMovementData();

private:
	static std::string my_client_id;

	std::map < std::string, std::vector<int> > allClients;
};
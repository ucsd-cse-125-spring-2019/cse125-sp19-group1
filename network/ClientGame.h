#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include "Player.h"
#include "StringParser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "GameData.h"


class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	std::map < std::string, std::vector<float> > clients2;

	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);
	void sendMovementPackets(const int type);

	char network_data[MAX_PACKET_SIZE];

	void update();


	std::map < std::string, Player > allClients;
	std::string my_client_id;

	int myID = -1;
	GameData * gameData;

	GameData * getGameData();
	int getMyID();
};
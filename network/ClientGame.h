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
#include <cstdint>

enum DirectionBitmask {
	westSide  = (0x1u << 3),  //1000 = left = 8
	northSide = (0x1u << 2),  //0100 = up = 4
	southSide = (0x1u << 1),  //0010 = down = 2
	eastSide  = (0x1u << 0),  //0001 = right = 1
	noSide    = 0,            //0000 = no wall = 0 
};

void loadMapArray(std::vector<std::vector<uint8_t>> &array, const char *filename);

class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();
	void decodeData(const char * data);
	void sendPackets(const int type, const int selectionNum = -1);

	char network_data[MAX_PACKET_SIZE];

	void update();


	int myID = -1;
	GameData * gameData;

	GameData * getGameData();
	int getMyID();

	//std::map < std::string, Player > allClients;
	//std::string my_client_id;

};
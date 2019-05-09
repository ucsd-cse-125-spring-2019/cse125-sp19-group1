#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Atlas.h"
#include <vector>
#include <algorithm>
//#include "Gate.h"

#define GENERALDATA_ID -999
#define SERVER_GAMEDATA 123

class GameData
{
public:
	GameData();
	GameData(int serverInit);

	std::vector<Location> initLocs =
	{
		Location(5 + TILE_SIZE * 0, 0, 5 + TILE_SIZE * 0),
		Location(5 + TILE_SIZE * 2, 0, 5 + TILE_SIZE * 0),
		Location(5 + TILE_SIZE * 0, 0, 5 + TILE_SIZE * 2),
		Location(5 + TILE_SIZE * 2, 0, 5 + TILE_SIZE * 2),
	};
	int initIndex = 0;

	std::map < int, Player * > players;
	Atlas * atlas;

	std::vector<std::vector<Tile *>> clientTileLayout;
	void addNewClient(int anID, Location aLoc);
	void removeClient(int anID);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData();
	void decodeGameData(const char * data);
	void addDecodeFunctions();

	void decodeTileLayout(std::string value);

	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();

	std::vector<std::vector<Tile *>> getTileLayout();

	int	getBoxTime() { return timeToOpenBox; }
	int getChefSwingTime() { return timeToSwingNet; }
	double getOpenJailTime() { return timeToOpenJail; }
	int timeToOpenBox = 2; //in seconds
	int timeToSwingNet = 1;
	double timeToOpenJail = 1.5;

protected:

private:
};
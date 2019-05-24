#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Atlas.h"
#include <chrono>
#include <vector>
#include <algorithm>

#include "GameConfigs.h"
//#include "Gate.h"

#define GENERAL_GAME_DATA_ID -999
#define SERVER_GAMEDATA 123

enum class ClientType { SERVER_SIDE, CLIENT_SIDE};

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
	void addNewPlayer(int anID, Location aLoc, ClientType type);
	void removePlayer(int anID, ClientType type);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData(bool newPlayerInit);
	void decodeGameData(const char * data);
	void addDecodeFunctions();

	void decodeTileLayout(std::string value);

	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();

	void updateGateProgress(int gateNum);

	std::vector<std::vector<Tile *>> getTileLayout();

	//int	getBoxTime() { return timeToOpenBox; }
	//int getChefSwingTime() { return timeToSwingNet; }
	//double getOpenJailTime() { return timeToOpenJail; }

	//int timeToOpenBox = 2; //in seconds
	//int timeToSwingNet = 1;
	//double timeToOpenJail = 1.5;

	int	chefAnger = 0;
	void incrementChefAnger() { chefAnger++; }

	std::chrono::time_point<std::chrono::system_clock> gameClock;
	void startGameClock();
	int getGameClock();

	void startCountdown();
	bool countdownDone();
	bool countdownStarted();

	// tile getters
	Tile * getTile(Location loc);
	GateTile * getGateTile(Location loc);
	KeyDropTile * getKeyDropTile(Location loc);
	BoxTile * getBoxTile(Location loc);
	RampTile * getRampTile(Location loc);
	JailTile * getJailTile(Location loc);
	ObjectTile * getAdjacentObjectTile(Location loc, Direction dir);
	Tile * getAdjacentTile(Location loc, Direction dir);
	JailTile * getAdjacentJailTile(Location loc, Direction dir);
	ObjectTile * getObjectTile(Location loc);
protected:
	bool beginCountdown;
	std::chrono::time_point<std::chrono::system_clock> countdownStartTime;
	bool countdownCompleted;
private:
};
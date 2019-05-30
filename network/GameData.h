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
#include <queue>
#include "GameConfigs.h"
//#include "Gate.h"

#define GENERAL_GAME_DATA_ID -999
#define SERVER_GAMEDATA 123

#define GHOST_MULTIPLIER 2
#define MAX_CHEF_TIME 7
#define MAX_ANIMAL_GHOST_TIME 7
#define MAX_ANIMAL_SEARCH_TIME 7
#define FLASH_DISTANCE 14


enum class ClientType { SERVER_SIDE, CLIENT_SIDE};

enum class WinType { NONE = 0, DOOR = 1, TOILET = 2, VENT = 3, CHEF_WIN = 4 };

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

	std::vector<std::pair<int, Player *>> disconnectedPlayers;
	std::map < int, Player * > players;
	Atlas * atlas;

	std::vector<std::vector<Tile *>> clientTileLayout;
	void addNewPlayer(unsigned int & anID, Location aLoc, ClientType type);
	void removePlayer(int anID, ClientType type);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData(bool newPlayerInit);
	void decodeGameData(const char * data);
	void addDecodeFunctions();
	void decodeTileLayout(std::string value);
	void decodeDisconnectedClients(std::string value);

	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();

	void updateGateProgress(int gateNum);

	std::vector<std::vector<Tile *>> getTileLayout();

	//int	getBoxTime() { return timeToOpenBox; }
	//double getChefSwingTime() { return timeToSwingNet; }
	//double getOpenJailTime() { return timeToOpenJail; }

	//int timeToOpenBox = 2; //in seconds
	//double timeToSwingNet = 0.5;
	//double timeToOpenJail = 1.5;

	WinType wt = WinType::NONE;
	WinType getWT() { return wt; }
	void setWT(WinType newWT) { wt = newWT; };

	double abilityChargeTime = 0.7;
	double maxGhostTime = 7;
	double limitChefVision = 1;
	double maxChefLimitTime = 7;

	double getAbilityChargeTime() { return abilityChargeTime; }
	double getMaxGhostTime() { return maxGhostTime; }
	double getMaxChefLimitTime() { return maxChefLimitTime; }

	int	chefAnger = 0;
	int maxChefAnger = 60;
	int currentTime = -1;
	int chefAngerInterval = 1;
	double chefVision = 40;
	double chefMaxVision = 100;

	void setChefVisionLimit(int multiplier) { limitChefVision = multiplier; }
	double getChefVision() { return chefVision * limitChefVision; }
	double getChefMaxVision() { return chefMaxVision; }
	void incrementChefVision() { chefVision++; }
	void incrementChefAnger() { chefAnger++; }
	int getChefAngerInterval() { return chefAngerInterval; }
	int getChefAnger() { return chefAnger; }
	int getMaxAnger() { return maxChefAnger; }
	int getCurrentTime() { return currentTime; }
	void setCurrentTime() { currentTime = getGameClock(); }

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
	ObjectTile * getAdjacentObjectTile(Location loc, Direction dir, Location & tileLoc);
	Tile * getAdjacentTile(Location loc, Direction dir, Location & tileLoc);
	JailTile * getAdjacentJailTile(Location loc, Direction dir, Location & tileLoc);
	ObjectTile * getObjectTile(Location loc);

protected:
	bool beginCountdown;
	std::chrono::time_point<std::chrono::system_clock> countdownStartTime;
	bool countdownCompleted;
private:
};
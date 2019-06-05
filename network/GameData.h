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

#define GENERAL_GAME_DATA_ID -999
#define SERVER_GAMEDATA 123

enum class ClientType { SERVER_SIDE, CLIENT_SIDE};

enum class WinType { NONE = 0, DOOR = 1, TOILET = 2, VENT = 3, CHEF_WIN = 4 };

enum class GameState { IN_LOBBY, IN_GAME, LOADING};

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
	void addNewPlayer(unsigned int anID, Location aLoc, ClientType type);
	void removePlayer(int anID, ClientType type);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData(bool newPlayerInit);
	void decodeGameData(const char * data);
	
	void addDecodeFunctions();
	void decodeTileLayout(std::string value);
	void decodeDisconnectedClients(std::string value);
	void decodeGameState(std::string value);
	void decodeChefAnger(std::string value);
	void decodeChefVision(std::string value);
	void decodeWinType(std::string value);
	void decodeBlindChef(std::string value);
	void decodeSlowChef(std::string value);
	void decodeAllPlayersLoaded(std::string value);

	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();
	ModelType getAvailableCharacter();

	void updateGateProgress(int gateNum);

	const std::vector<std::vector<Tile *>>& getTileLayout() const;

	WinType wt = WinType::NONE;
	WinType getWT() { return wt; }
	void setWT(WinType newWT) { wt = newWT; };

	int currentTime = -1;

	int	chefAnger = 0;
	double chefVision = 40;
	double chefRampVision = 0;

	bool blindChef = false;
	bool slowChef = false;
	
	void setChefRampVision(double crv) { chefRampVision = crv; }
	double getChefRampVision() { return chefRampVision; }

	bool getBlindChef() { return blindChef; }
	void setBlindChef(bool aBlindChef) { blindChef = aBlindChef; }

	bool getSlowChef() { return slowChef; }
	void setSlowChef(bool aSlowChef) { slowChef = aSlowChef; }

	void incrementChefVision() { chefVision++; }
	double getChefVision();

	void incrementChefAnger() { chefAnger++; }
	int getChefAnger() { return chefAnger; }

	int getCurrentTime() { return currentTime; }
	void setCurrentTime() { currentTime = getGameClock(); }

	std::chrono::time_point<std::chrono::system_clock> gameClock;
	void startGameClock();
	int getGameClock();

	void startCountdown();
	bool countdownDone();
	bool countdownStarted();

	bool getAllPlayersLoaded();
	void checkAllPlayersLoaded();

	GameState getGameState();
	void setGameState(GameState state);

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
	GameState gameState;
	bool beginCountdown;
	std::chrono::time_point<std::chrono::system_clock> countdownStartTime;
	bool countdownCompleted;
	std::vector<ModelType> availableCharacters{ ModelType::RACOON, ModelType::CAT, ModelType::DOG };

	int playerNum;
	int doneLoadingCount;
	bool allPlayersLoaded;
private:
};
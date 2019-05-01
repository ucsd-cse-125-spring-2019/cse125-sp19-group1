#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Atlas.h"
#include <vector>
#include <algorithm>

#define GENERALDATA_ID -999
#define SERVER_GAMEDATA 123


enum class Key { KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE };

struct Gate
{
public:
	Gate(std::vector<Key> aKeys = std::vector<Key>({ Key::KEY1, Key::KEY2, Key::KEY3 }), int num = 0) : location(Location()), progress(0.0f), gateNum(num), validKeys(aKeys), isOpen(false) {}
	Gate(Location aLocation, int num = 0) : location(aLocation), progress(0.0f), gateNum(num), isOpen(false) {}
	
	Location	getLocation() { return location; }
	float		getProgress() { return progress; }
	int			getGateNum() { return gateNum; }

	void setProgress(float aProgress) { progress = aProgress; }

	bool isValidKey(Key aKey) { return std::find(validKeys.begin(), validKeys.end(), aKey) != validKeys.end(); }
	
	// Updates progress and removes the key so that duplicate keys cannot be used
	void updateProgress(Key aKey) {
		progress++;
		validKeys.erase(std::find(validKeys.begin(), validKeys.end(), aKey)); 
		if (progress == 3)
			isOpen = true;
	}

	std::string encodeGateData() {
		std::stringstream encodedData;
		encodedData << "num: " << gateNum << "|"
			<< "progress: " << progress << "|"
			<< "isOpen: " << isOpen << "|"
			<< "location: " << location.getX() << " " << location.getY() << " " << location.getZ()
			<< std::endl;
		return encodedData.str();
	}
	std::vector<Key> validKeys;
protected:
	Location location;
	float progress;
	int gateNum;
	std::vector<std::vector<int>> wallLayout;
	bool isOpen;
};

struct Item {

public:

	Item() : name(ItemName::EMPTY), holdStatus(false), location(Location()), droppedTime(0) {}
	Item(ItemName anItem, Location aLoc) :name(anItem), location(aLoc), holdStatus(false), droppedTime(0) {}

	ItemName getName() { return name; }
	bool isHeld() { return holdStatus; }
	Location getLocation() { return location; }
	int getDroppedTime() { return droppedTime; }
	
	void setLocation(Location aLoc) { location = aLoc; }
	void setDroppedTime(int aTime) { droppedTime = aTime; }

protected:
	ItemName name;
	bool holdStatus;
	Location location;
	int droppedTime;
};
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
	Gate gate1;

	std::vector<std::vector<int>> clientWallLayout;
	std::vector<std::vector<int>> clientKeyLayout;
	std::vector<std::vector<int>> clientGateLayout;
	std::vector<std::vector<int>> clientBoxLayout;
	std::vector<std::vector<Tile>> clientTileLayout;
	void addNewClient(int anID, Location aLoc);
	void removeClient(int anID);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData();
	void decodeGameData(const char * data);
	void addDecodeFunctions();
	void decodeWallLayout(std::string value);
	void decodeKeyLayout(std::string value);
	void decodeGateLayout(std::string value);
	void decodeBoxLayout(std::string value);
	void decodeTileLayout(std::string value);

	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();
	Gate & getGate();
	std::vector<std::vector<int>> & getWallLayout();
	std::vector<std::vector<int>> & getKeyLayout();
	std::vector<std::vector<int>> & getGateLayout();
	std::vector<std::vector<int>> & getBoxLayout();

	int	getBoxTime() { return timeToOpenBox; }
	int timeToOpenBox = 2; //in seconds

protected:

private:
};
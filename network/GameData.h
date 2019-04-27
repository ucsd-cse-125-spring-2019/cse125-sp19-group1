#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Walls.h"
#include "Atlas.h"
#include <vector>

#define GENERALDATA_ID -999

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

class GameData
{
public:
	GameData();
	GameData(Walls * aPtr);

	std::map < int, Player * > players;
	Walls * walls;
	Atlas * atlas;
	std::vector<std::vector<int>> wallLayout;
	Gate gate1;

	void addNewClient(int anID, Location aLoc);
	void removeClient(int anID);

	using decodeFunctionType = void (GameData::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	std::string encodeGameData();
	void decodeGameData(const char * data);
	void addDecodeFunctions();
	Player * getPlayer(int anID);
	std::map < int, Player * > & getAllPlayers();
	Atlas * getAtlas();

	Gate & getGate();

protected:

private:
};
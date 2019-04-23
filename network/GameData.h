#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Walls.h"
#include <vector>

#define GENERALDATA_ID -10

enum class Keys { KEY1 = 100, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9 };

struct Gate
{
public:
	//Gate(int num = 0) : location(Location()), progress(0.0f), gateNum(num), validKeys(aKeys) {})
	Gate(std::vector<Keys> aKeys = std::vector<Keys>({ Keys::KEY1, Keys::KEY2, Keys::KEY3 }), int num = 0) : location(Location()), progress(0.0f), gateNum(num), validKeys(aKeys) {}
	Gate(Location aLocation, int num = 0) : location(aLocation), progress(0.0f), gateNum(num) {}
	
	Location	getLocation() { return location; }
	float		getProgress() { return progress; }
	int			getGateNum() { return gateNum; }

	void setProgress(float aProgress) { progress = aProgress; }

	bool isValidKey(Keys aKey) { return std::find(validKeys.begin(), validKeys.end(), aKey) != validKeys.end(); }
	// Updates progress and removes the key so that duplicate keys cannot be used
	void updateProgress(Keys aKey) {
		progress++;
		validKeys.erase(std::find(validKeys.begin(), validKeys.end(), aKey)); 
	}
	std::vector<Keys> validKeys;
protected:
	Location location;
	float progress;
	int gateNum;
	
};

class GameData
{
public:
	GameData();
	GameData(Walls * aPtr);

	std::map < int, Player * > clients;
	Walls * walls;
	int ** layout;
	Gate gate1;

	void addNewClient(int anID);

	//typedef std::string(*encodeFunctionType)(std::string key);
	//std::map<std::string, encodeFunctionType> encodingFunctions;
	std::string encodeGameData();
	void decodeGameData();


protected:

private:
};
#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>
#include "Walls.h"

#define GENERALDATA_ID -10

struct Gate
{
public:
	Location getLocation() { return location; }
	float getProgress() { return progress; }
protected:
	Location location;
	float progress;
};

class GameData
{
public:
	GameData();
	GameData(Walls * aPtr);

	std::map < int, Player * > clients;
	Walls * walls;
	int ** layout;

	void addNewClient(int anID);

	typedef std::string(*encodeFunctionType)(std::string key);
	std::map<std::string, encodeFunctionType> encodingFunctions;
	std::string encodeGameData();
	void decodeGameData();


protected:

private:
};
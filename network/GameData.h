#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"
#include <sstream>
#include <string>

class GameData
{
public:
	GameData();

	std::map < int, Player > clients;

	typedef std::string(*encodeFunctionType)(std::string key);

	void addNewClient(int anID);
	std::map<std::string, encodeFunctionType> encodingFunctions;
	std::string encodeLocation(std::string key);
	std::string encodeGameData();

protected:

private:
};
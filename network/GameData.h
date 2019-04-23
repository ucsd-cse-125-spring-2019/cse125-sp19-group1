#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"

class GameData
{
public:
	GameData();

	std::map < std::string, Player > clients;

	typedef std::string(*encodeFunctionType)(std::string key);

	std::map<std::string, encodeFunctionType> encodingFunctions;
	std::string encodeLocation(std::string key);

protected:

private:
};
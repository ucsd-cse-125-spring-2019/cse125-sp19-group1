#pragma once
#include <map>
#include "Player.h"
#include "StringParser.h"

class GameData
{
public:
	GameData();

	std::map < std::string, Player > allClients;

protected:

private:
};
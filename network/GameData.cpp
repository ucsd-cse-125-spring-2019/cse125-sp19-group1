#include "GameData.h"

GameData::GameData()
{

}

std::string GameData::encodeGameData()
{
	std::stringstream encodedData;

	for (auto iter = clients.begin(); iter != clients.end(); iter++)
	{
		encodedData << iter->second.encodePlayerData();
	}
	return encodedData.str();
}

void GameData::addNewClient(int anID)
{
	clients[anID] = Player(anID);
}
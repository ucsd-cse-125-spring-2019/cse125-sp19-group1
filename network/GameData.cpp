#include "GameData.h"

GameData::GameData()
{
	walls = nullptr;
}

GameData::GameData(Walls * aPtr)
{
	walls = aPtr;
	//layout = walls->layout;
}

std::string GameData::encodeGameData()
{
	std::stringstream encodedData;

	for (auto iter = clients.begin(); iter != clients.end(); iter++)
	{
		encodedData << iter->second->encodePlayerData();
	}
	encodedData << "client: " << GENERALDATA_ID << std::endl;
	encodedData << "walls: " << walls->encodeWallData();
	encodedData << "gate: " << gate1.encodeGateData();

	return encodedData.str();
}

void GameData::addNewClient(int anID)
{
	clients[anID] = new Player(anID);
}

void GameData::decodeGameData()
{

}
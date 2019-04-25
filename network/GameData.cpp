#include "GameData.h"

GameData::GameData()
{
	walls = new Walls();
}

GameData::GameData(Walls * aPtr)
{
	walls = aPtr;
	//layout = walls->layout;
}

std::string GameData::encodeGameData()
{
	std::stringstream encodedData;
	encodedData.clear();

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

void GameData::decodeGameData(const char * data)
{
	std::vector<std::pair<std::string, std::string>> keyValuePairs;
	keyValuePairs = StringParser::parseKeyValueString(data);

	for (auto p : keyValuePairs)
	{
		std::cout << p.first << " : " << p.second << std::endl;
	}

}
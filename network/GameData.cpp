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

	for (auto iter = players.begin(); iter != players.end(); iter++)
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
	players[anID] = new Player(anID);
}

void GameData::decodeGameData(const char * data)
{
	std::vector<std::pair<std::string, std::string>> keyValuePairs;
	keyValuePairs = StringParser::parseKeyValueString(data);

	int playerID = -1;
	for (auto p : keyValuePairs)
	{
		if (p.first == "client")
		{
			playerID = std::stoi(p.second);
			if (players.count(playerID) == 0)
			{
				addNewClient(playerID);
			}
			std::cout << p.first << " : " << p.second << std::endl;

		}
		else
		{


			std::cout << p.first << " : " << p.second << std::endl;

			if (playerID == GENERALDATA_ID)
			{

			}
			else
			{
				if (players.count(playerID) > 0)
				{
					players[playerID]->decodePlayerData(p.first, p.second);
				}
			}
		}
	}
}
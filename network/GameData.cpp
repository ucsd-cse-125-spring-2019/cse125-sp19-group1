#include "GameData.h"

GameData::GameData()
{
	walls = new Walls();
	addDecodeFunctions();
}

GameData::GameData(Walls * aPtr)
{
	walls = aPtr;
	//layout = walls->layout;
	addDecodeFunctions();
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

void GameData::removeClient(int anID)
{
	players.erase(anID);
}

void GameData::addDecodeFunctions()
{

}

Player * GameData::getPlayer(int anID)
{
	if (players.count(anID) > 0)
		return players[anID];
	else
		return nullptr;
}

std::map < int, Player * > & GameData::getAllPlayers()
{
	return players;
}
void GameData::decodeGameData(const char * data)
{
	std::vector<std::pair<std::string, std::string>> keyValuePairs;
	keyValuePairs = StringParser::parseKeyValueString(data);

	int playerID = -1;
	for (auto p : keyValuePairs)
	{
		std::string & key = p.first;
		std::string & value = p.second;
		if (p.first == "client")
		{
			playerID = std::stoi(value);
			if (players.count(playerID) == 0 && playerID != GENERALDATA_ID)
			{
				addNewClient(playerID);
			}
			//std::cout << key << " : " << value << std::endl;

		}
		else
		{


			//std::cout << key << " : " << value << std::endl;

			if (playerID == GENERALDATA_ID)
			{
				if (decodingFunctions.count(key) > 0)
					(this->*decodingFunctions[key])(value); // Format for calling the functions from the map
				else
					std::cout << "No decoding function for key: " << key << std::endl;
			}
			else
			{
				// Makes sure this player ID exists before trying to decode data
				if (players.count(playerID) > 0)
				{
					players[playerID]->decodePlayerData(key, value);
				}
			}
		}
	}
}
#include "GameData.h"

GameData::GameData()
{
	atlas = nullptr;
	addDecodeFunctions();
}

GameData::GameData(int serverInit)
{
	atlas = new Atlas();
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
	encodedData << "wallLayout: " << atlas->encodeWallLayoutData();
	encodedData << "keyLayout: " << atlas->encodeClientKeyLayoutData();
	encodedData << "gateLayout: " << atlas->encodeGateLayoutData();
	encodedData << "boxLayout: " << atlas->encodeBoxLayoutData();
	encodedData << "gate: " << gate1.encodeGateData();

	return encodedData.str();
}

void GameData::addNewClient(int anID, Location aLoc)
{
	players[anID] = new Player(anID, initLocs[anID % initLocs.size()]);
}

void GameData::removeClient(int anID)
{
	players.erase(anID);
}

void GameData::addDecodeFunctions()
{
	decodingFunctions["wallLayout"] = &GameData::decodeWallLayout;
	decodingFunctions["keyLayout"] = &GameData::decodeKeyLayout;
	decodingFunctions["gateLayout"] = &GameData::decodeGateLayout;
	decodingFunctions["boxLayout"] = &GameData::decodeBoxLayout;
}

void GameData::decodeWallLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');

	clientWallLayout = StringParser::parse2DIntArrayString(value);
}
void GameData::decodeKeyLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');
	clientKeyLayout = StringParser::parse2DIntArrayString(value);
}
void GameData::decodeGateLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');
	clientGateLayout = StringParser::parse2DIntArrayString(value);
}
void GameData::decodeBoxLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');
	clientBoxLayout = StringParser::parse2DIntArrayString(value);
}

Player * GameData::getPlayer(int anID)
{
	if (players.count(anID) > 0)
		return players[anID];
	else
		return nullptr;
}

std::map < int, Player * > & GameData::getAllPlayers() { return players; }
Atlas * GameData::getAtlas() { return atlas; }
Gate & GameData::getGate() { return gate1; }

std::vector<std::vector<int>> & GameData::getWallLayout() { return clientWallLayout; }
std::vector<std::vector<int>> & GameData::getKeyLayout() { return clientKeyLayout; }
std::vector<std::vector<int>> & GameData::getGateLayout() { return clientGateLayout; }
std::vector<std::vector<int>> & GameData::getBoxLayout() { return clientBoxLayout; }

void GameData::decodeGameData(const char * data)
{
	std::vector<std::pair<std::string, std::string>> keyValuePairs;
	keyValuePairs = StringParser::parseKeyValueString(data);

	int playerID = -1;
	for (auto p : keyValuePairs)
	{
		std::string & key = p.first;
		std::string & value = p.second;
		//std::cout << key << " : " << value << std::endl;
		if (p.first == "client")
		{
			playerID = std::stoi(value);
			if (players.count(playerID) == 0 && playerID != GENERALDATA_ID)
			{
				addNewClient(playerID, Location());
			}
		}
		else
		{
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
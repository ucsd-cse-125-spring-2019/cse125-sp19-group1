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
	encodedData << "tileLayout: " << atlas->encodeTileLayoutData();
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
	decodingFunctions["tileLayout"] = &GameData::decodeTileLayout;
}
void GameData::decodeTileLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');

	//clientTileLayout.clear();
	bool init = false;
	if (clientTileLayout.size() == 0)
		init = true;

	std::vector<std::pair<std::string, std::string>> tileDataPairs = StringParser::parseKeyValueString(value.c_str());

	int row = -1;
	int col = -1;
	std::vector<Tile> tileRow;
	for (auto p : tileDataPairs)
	{
		if (p.first == "tile")
		{
			std::stringstream tmpStream(p.second);
			std::string r, c;

			tmpStream >> r >> c;
			row = std::stoi(r);
			col = std::stoi(c);
		}
		else if (p.first == "tileData" && row != -1 && col != -1)
		{
			std::stringstream tileDataStream(p.second);
			std::string wallLayout_str, height_str, tileType_str, boxStatus_str, itemName_str;

			tileDataStream >> wallLayout_str >> height_str >> tileType_str >> boxStatus_str >> itemName_str;

			int wallLayout = std::stoi(wallLayout_str);
			int height = std::stoi(height_str);
			TileType tileType = static_cast<TileType>(std::stoi(tileType_str));
			bool boxStatus = boxStatus_str == "1";
			ItemName itemName = static_cast<ItemName>(std::stoi(itemName_str));

			Tile tmp(wallLayout, tileType, boxStatus, itemName, height);
			if (init)
			{
				tileRow.push_back(tmp);
			}
			else
			{
				clientTileLayout[row][col] = tmp;
			}
		}
		else if (p.first == "newRow")
		{
			if (init)
			{
				clientTileLayout.push_back(tileRow);
				tileRow.clear();
			}
		}

	}

	/*while (std::getline(valueStream, line))
	{
		std::replace(line.begin(), line.end(), ',', '\n');

		std::stringstream lineStream(line);
		std::string tileData;
		std::vector<Tile> tileRow;
		while (std::getline(lineStream, tileData))
		{
			std::replace(tileData.begin(), tileData.end(), '/', '\n');
			std::vector<std::pair<std::string, std::string>> tileDataPairs = StringParser::parseKeyValueString(tileData.c_str());
			
			int row = std::stoi(tileDataPairs[0].second);
			int col = std::stoi(tileDataPairs[1].second);

			tileData = tileDataPairs[2].second;
			std::stringstream tileDataStream(tileData);
			std::string wallLayout_str, height_str, tileType_str, boxStatus_str, itemName_str;

			tileDataStream >> wallLayout_str >> height_str >> tileType_str >> boxStatus_str >> itemName_str;

			int wallLayout = std::stoi(wallLayout_str);
			int height = std::stoi(height_str);
			TileType tileType = static_cast<TileType>(std::stoi(tileType_str));
			bool boxStatus = boxStatus_str == "1";
			ItemName itemName = static_cast<ItemName>(std::stoi(itemName_str));

			Tile tmp(wallLayout, tileType, boxStatus, itemName, height);
			if (init)
			{
				tileRow.push_back(tmp);
			}
			else
			{
				clientTileLayout[row][col] = tmp;
			}
		}
		if (init)
		{
			clientTileLayout.push_back(tileRow);
		}
	}*/

	std::cout << "SFS\n";
	for (auto p : clientTileLayout)
	{
		for (auto c : p)
		{
			std::cout << c.getWall() << " ";
		}
		std::cout << std::endl;
	}
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
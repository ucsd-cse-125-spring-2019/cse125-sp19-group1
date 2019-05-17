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

std::string GameData::encodeGameData(bool newPlayerInit)
{
	std::stringstream encodedData;
	encodedData.clear();

	for (auto iter = players.begin(); iter != players.end(); iter++)
	{
		encodedData << iter->second->encodePlayerData(newPlayerInit);
	}
	encodedData << "client: " << GENERALDATA_ID << std::endl;
	encodedData << "tileLayout: " << atlas->encodeTileLayoutData(newPlayerInit);

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
	decodingFunctions["tileLayout"] = &GameData::decodeTileLayout;
}
void GameData::decodeTileLayout(std::string value)
{
	std::replace(value.begin(), value.end(), '|', '\n');
	std::vector<std::pair<std::string, std::string>> tileDataPairs = StringParser::parseKeyValueString(value.c_str());

	bool init = false;
	if (clientTileLayout.size() == 0)
		init = true;


	int row = -1;
	int col = -1;
	TileType type = TileType::DEFAULT;
	std::vector<Tile *> tileRow;
	for (auto p : tileDataPairs)
	{
		if (p.first == "tile")
		{
			std::stringstream tmpStream(p.second);
			std::string r, c;

			tmpStream >> r >> c;
			row = std::stoi(r);
			col = std::stoi(c);
			type = TileType::DEFAULT; // reset type to be default
		}
		else if (p.first == "tileType")
		{
			std::stringstream tmpStream(p.second);
			std::string typeStr;

			tmpStream >> typeStr;
			type = static_cast<TileType>(std::stoi(typeStr));
		}
		else if (p.first == "tileData" && row != -1 && col != -1)
		{
			if (init)
			{
				Tile * tmp = nullptr;// = new Tile();
				
				switch (type)
				{
				case TileType::BOX:
					tmp = new BoxTile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::JAIL: // change to JailTile
					tmp = new Tile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::GATE:
					tmp = new GateTile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::RAMP:
					tmp = new RampTile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::KEY_DROP: // change to KeyDropTile
					tmp = new Tile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::TABLE: // change to ObjectTile
					tmp = new Tile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::DEFAULT: default:
					tmp = new Tile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				}
			}
			else
			{
				clientTileLayout[row][col]->decodeTileData(p.second);
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

	// Debug printing
	/*for (auto p : clientTileLayout)
	{
		for (auto c : p)
		{
			std::cout << c.getWall() << " ";
		}
		std::cout << std::endl;
	}*/
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

std::vector<std::vector<Tile *>> GameData::getTileLayout() { return clientTileLayout; }
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

int GameData::getGameClock()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - gameClock;
	return (int)elapsed_seconds.count();
}


void GameData::startGameClock()
{
	gameClock = std::chrono::system_clock::now();
}
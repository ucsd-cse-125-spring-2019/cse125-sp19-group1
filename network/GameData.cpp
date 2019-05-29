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
	beginCountdown = false;
	countdownCompleted = false;
}

std::string GameData::encodeGameData(bool newPlayerInit)
{
	std::stringstream encodedData;
	encodedData.clear();

	for (auto iter = players.begin(); iter != players.end(); iter++)
	{
		encodedData << iter->second->encodePlayerData(newPlayerInit);
	}
	encodedData << "client: " << GENERAL_GAME_DATA_ID << std::endl;

	if (beginCountdown)
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - countdownStartTime;
		encodedData << "count down: " << 5.0 - elapsed_seconds.count() << std::endl;
	}
	encodedData << "tileLayout: " << atlas->encodeTileLayoutData(newPlayerInit);
	encodedData << "chefAnger: " << getChefAnger() << std::endl;
	encodedData << "chefVision:" << getChefVision() << std::endl;
	encodedData << "winType: " << (int) getWT() << std::endl;
	
	//std::cout << encodedData.str() << std::endl;
	return encodedData.str();
}

void GameData::updateGateProgress(int gateNum)
{
	getAtlas()->updateGateProgress(gateNum);
}

void GameData::startCountdown()
{
	countdownStartTime = std::chrono::system_clock::now();
	beginCountdown = true;
}
bool GameData::countdownStarted()
{
	return beginCountdown;
}
bool GameData::countdownDone()
{
	if (beginCountdown)
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - countdownStartTime;

		if (5.0 - elapsed_seconds.count() < 0)
		{
			beginCountdown = false;
			countdownCompleted = true;
			return true;
		}
	}
	return countdownCompleted;
}
void GameData::addNewPlayer(int anID, Location aLoc, ClientType type)
{
	if (type == ClientType::SERVER_SIDE)
	{
		if (disconnectedPlayers.size() > 0)
		{
			players[anID] = disconnectedPlayers.front().second;
			disconnectedPlayers.pop();
		}
		else
			players[anID] = new Player(anID, initLocs[anID % initLocs.size()]);
	}
	else if (type == ClientType::CLIENT_SIDE)
	{
		players[anID] = new Player(anID, initLocs[anID % initLocs.size()]);

	}
}

void GameData::removePlayer(int anID, ClientType type)
{
	if (type == ClientType::SERVER_SIDE)
	{
		disconnectedPlayers.push(std::pair<int, Player *>(anID, players.at(anID)));
	}
	else if (type == ClientType::CLIENT_SIDE)
	{

	}
	if(players.count(anID) > 0)
		players.erase(anID);
}

void GameData::addDecodeFunctions()
{
	decodingFunctions["tileLayout"] = &GameData::decodeTileLayout;
	decodingFunctions["disconnectedClients"] = &GameData::decodeDisconnectedClients;
}

void GameData::decodeDisconnectedClients(std::string value)
{
	std::stringstream valueStream(value);
	std::string id_str;

	// Get values from the stream
	while (valueStream >> id_str)
	{
		removePlayer(std::stoi(id_str), ClientType::CLIENT_SIDE);
	}
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
					tmp = new JailTile();
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
					tmp = new KeyDropTile();
					tmp->decodeTileData(p.second);
					tileRow.push_back(tmp);
					break;
				case TileType::OBJECT: // change to ObjectTile
					tmp = new ObjectTile();
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
			if (players.count(playerID) == 0 && playerID != GENERAL_GAME_DATA_ID)
			{
				addNewPlayer(playerID, Location(), ClientType::CLIENT_SIDE);
			}
		}
		else if (p.first == "chefAnger") {
			chefAnger = std::stoi(value);
		}
		else if (p.first == "chefVision") {
			chefVision = std::stoi(value);
		}
		else if (p.first == "winType") {
			wt = (WinType)std::stoi(value);
		}
		else
		{
			if (playerID == GENERAL_GAME_DATA_ID)
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

// tile getters
Tile * GameData::getTile(Location loc)
{
	if (atlas)
	{
		return atlas->getTileAt(loc);
	}
	else
	{
		int row, col;
		Atlas::getMapCoords(loc, row, col);
		if (row >= 0 && row < clientTileLayout.size() && col >= 0 && col < clientTileLayout[row].size())
			return clientTileLayout[row][col];
		else
			return nullptr;
	}
}
GateTile * GameData::getGateTile(Location loc)
{
	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::GATE)
			return dynamic_cast<GateTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
	
}
KeyDropTile * GameData::getKeyDropTile(Location loc)
{
	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::KEY_DROP)
			return dynamic_cast<KeyDropTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
	
}
BoxTile * GameData::getBoxTile(Location loc)
{
	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::BOX)
			return dynamic_cast<BoxTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
}
RampTile * GameData::getRampTile(Location loc)
{
	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::RAMP)
			return dynamic_cast<RampTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
}
JailTile * GameData::getJailTile(Location loc)
{

	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::JAIL)
			return dynamic_cast<JailTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
}

ObjectTile * GameData::getObjectTile(Location loc)
{

	if (Tile * tile = getTile(loc))
	{
		if (tile->getTileType() == TileType::JAIL)
			return dynamic_cast<ObjectTile*>(tile);
		else
			return nullptr;
	}
	return nullptr;
}
Tile * GameData::getAdjacentTile(Location loc, Direction dir, Location & tileLoc)
{
	float increment = TILE_SIZE / 2;
	switch (dir)
	{
	case Direction::NORTH:
		tileLoc = Location(loc.getX(), loc.getY(), loc.getZ() + increment);
		break;
	case Direction::SOUTH:
		tileLoc = Location(loc.getX(), loc.getY(), loc.getZ() - increment);

		break;
	case Direction::EAST:
		tileLoc = Location(loc.getX() - increment, loc.getY(), loc.getZ());

		break;
	case Direction::WEST:
		tileLoc = Location(loc.getX() + increment, loc.getY(), loc.getZ());

		break;
	case Direction::NORTHEAST:
		tileLoc = Location(loc.getX() - increment, loc.getY(), loc.getZ() + increment);

		break;
	case Direction::NORTHWEST:
		tileLoc = Location(loc.getX() + increment, loc.getY(), loc.getZ() + increment);

		break;
	case Direction::SOUTHEAST:
		tileLoc = Location(loc.getX() - increment, loc.getY(), loc.getZ() - increment);

		break;
	case Direction::SOUTHWEST:
		tileLoc = Location(loc.getX() + increment, loc.getY(), loc.getZ() - increment);

		break;
	}

	return getTile(tileLoc);
}

ObjectTile * GameData::getAdjacentObjectTile(Location loc, Direction dir, Location & tileLoc)
{
	Tile * tile = getAdjacentTile(loc, dir, tileLoc);

	if (tile && tile->getTileType() == TileType::OBJECT)
	{
		return dynamic_cast<ObjectTile *>(tile);
	}
	else
		return nullptr;
}

JailTile * GameData::getAdjacentJailTile(Location loc, Direction dir, Location & tileLoc)
{
	Tile * tile = getAdjacentTile(loc, dir, tileLoc);

	if (tile && tile->getTileType() == TileType::JAIL)
	{
		std::bitset<4> wall(tile->getWall());
		// 3 EAST
		// 2 SOUTH
		// 1 NORTH
		// 0 WEST

		// Check if there is a wall in between the player and jail
		switch (dir)
		{
		case Direction::NORTH:
			if (wall[2])
				return nullptr;
			break;
		case Direction::SOUTH:
			if (wall[1])
				return nullptr;
			break;
		case Direction::EAST:
			if (wall[0])
				return nullptr;
			break;
		case Direction::WEST:
			if (wall[3])
				return nullptr;
			break;
		case Direction::NORTHEAST:
			if (wall[2] && wall[0])
				return nullptr;
			break;
		case Direction::NORTHWEST:
			if (wall[2] && wall[3])
				return nullptr;
			break;
		case Direction::SOUTHEAST:
			if (wall[1] && wall[0])
				return nullptr;
			break;
		case Direction::SOUTHWEST:
			if (wall[1] && wall[3])
				return nullptr;
			break;
		}
		return dynamic_cast<JailTile *>(tile);
	}
	else
		return nullptr;
}

#include "Atlas.h"

//1000 = left = 8
//0100 = up = 4
//0010 = down = 2
//0001 = right = 1
//0000 = no wall = 0 

void Atlas::getMapCoords(Location & loc, int & row, int & col)
{
	row = (int)(loc.getZ() / TILE_SIZE);
	col = (int)(loc.getX() / TILE_SIZE);
}

Atlas::Atlas()
{
	//Reading from a file to generate map
	//std::ifstream wallFile("layout.txt");
	//std::ifstream boxFile("box.txt");
	std::ifstream boxFile("../../maps/tinytinymap/item_spawn.txt");
	std::ifstream wallFile("../../maps/tinytinymap/walls.txt");
	std::ifstream heightFile("../../maps/tinytinymap/heights.txt");
	std::ifstream gateFile("../../maps/tinytinymap/player_exit.txt");
	std::ifstream jailFile("../../maps/tinytinymap/player_trap.txt");
	std::ifstream rampFile("../../maps/tinytinymap/ramps.txt");
	std::ifstream keyDepositFile("../../maps/tinytinymap/key_deposit.txt");

	std::string wallLine;
	std::string boxLine;
	std::string gateLine;
	std::string heightLine;
	std::string jailLine;
	std::string rampLine;
	std::string keyDepositLine;

	//printf("INITIALIZING WALLS!\n");

	std::getline(wallFile, wallLine); // removes first line from file
	std::getline(boxFile, boxLine); // removes first line from file
	std::getline(gateFile, gateLine); // removes first line from file
	std::getline(heightFile, heightLine); // removes first line from file
	std::getline(jailFile, jailLine); // removes first line from file
	std::getline(rampFile, rampLine); // removes first line from file
	std::getline(keyDepositFile, keyDepositLine); // removes first line from file
	int row = 0;
	while (std::getline(wallFile, wallLine))
	{
		int col = 0;
		std::getline(boxFile, boxLine);
		std::getline(gateFile, gateLine);
		std::getline(heightFile, heightLine);
		std::getline(jailFile, jailLine);
		std::getline(rampFile, rampLine);
		std::getline(keyDepositFile, keyDepositLine);
		std::stringstream wallStream(wallLine);
		std::stringstream boxStream(boxLine);
		std::stringstream gateStream(gateLine);
		std::stringstream heightStream(heightLine);
		std::stringstream jailStream(jailLine);
		std::stringstream rampStream(rampLine);
		std::stringstream keyDepositStream(keyDepositLine);
		std::string wallNum;
		std::string boxNum;
		std::string gateNum;
		std::string heightNum;
		std::string jailNum;
		std::string rampNum;
		std::string keyDepositNum;
		std::vector<Tile *> tileRow;
		std::vector<int> keyLocationsRow;
		while (wallStream >> wallNum)
		{
			// Read next num from the line
			boxStream >> boxNum;
			gateStream >> gateNum;
			heightStream >> heightNum;
			jailStream >> jailNum;
			rampStream >> rampNum;
			keyDepositStream >> keyDepositNum;

			// Initialize default variables for a tile
			TileType type(TileType::DEFAULT);
			int wall = std::stoi(wallNum);
			int height = std::stoi(heightNum);

			if (boxNum != "0")
			{
				type = TileType::BOX;
			}
			else if (gateNum != "0")
			{
				type = TileType::GATE;
			}
			else if (jailNum != "0")
			{
				type = TileType::JAIL;
			}
			else if (rampNum != "0")
			{
				type = TileType::RAMP;
			}
			else if (keyDepositNum != "0")
			{
				type = TileType::KEY_DROP;
			}

			switch (type)
			{
			case TileType::BOX:
				tileRow.push_back(new BoxTile(wall, height));
				boxLocations.push_back(std::pair<int, int>(row, col));
				break;
			case TileType::JAIL: // change to JailTile
				tileRow.push_back(new Tile(TileType::JAIL, wall, height));
				break;
			case TileType::GATE: 
				tileRow.push_back(new GateTile(std::vector<Key>({ Key::KEY1, Key::KEY2, Key::KEY3 }), 1, wall, height));
				break;
			case TileType::RAMP:
				tileRow.push_back(new RampTile(static_cast<Orientation>(std::stoi(rampNum)), wall, height));
				break;
			case TileType::KEY_DROP: // change to KeyDropTile
				tileRow.push_back(new Tile(TileType::KEY_DROP, wall, height));
				break;
			case TileType::TABLE: // change to ObjectTile
				tileRow.push_back(new Tile(TileType::TABLE, wall, height));
				break;
			case TileType::DEFAULT: default:
				tileRow.push_back(new Tile(TileType::DEFAULT, wall, height));
				break;

			}
			// Populate keyLocations 2D vector
			keyLocationsRow.push_back(0);
			col++;
		}
		tileLayout.push_back(tileRow);
		keyLocations.push_back(keyLocationsRow);
		row++;
	}


	// Debug print out the wall layout
	for (int r = 0; r < tileLayout.size(); r++) {
		for (int c = 0; c < tileLayout[r].size(); c++) {

			std::cout << tileLayout[r][c]->getWall() << " ";
		}
		std::cout << std::endl;
	}
	srand(time(NULL));
	// Remove extra boxes locations to use until box count matches item count
	while (boxLocations.size() > MAX_ITEMS)
	{
		int randPos = rand() % boxLocations.size();
		boxLocations.erase(boxLocations.begin() + randPos);
	}

	for(auto boxLocationsIter = boxLocations.begin(); boxLocationsIter != boxLocations.end(); boxLocationsIter++)
	{
		// Gets row and col location of the box
		int row = boxLocationsIter->first;
		int col = boxLocationsIter->second;
		ItemName randItem = ItemName::EMPTY;
		
		// Get random item that is still available
		while(std::find(itemList.begin(), itemList.end(), randItem) == itemList.end())
			randItem = static_cast<ItemName>(rand() % MAX_ITEMS + 1);

		itemsMap.emplace(randItem, Item(randItem, row, col));
		keyLocations[row][col] = static_cast<int>(randItem);

		// Removes the item from the item list
		itemList.erase(std::find(itemList.begin(), itemList.end(), randItem));
	}

	std::cout << "keys" << std::endl;
	// Debug print key layout
	for (auto v : keyLocations)
	{
		for (auto x : v)
		{
			std::cout << x << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "end atlas constructor\n";
}

void Atlas::detectCollision(Location & loc) {
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size())
		row = tileLayout.size() - 1;

	else if (row < 0)
		row = 0;

	if (col >= tileLayout[row].size())
	{
		col = tileLayout[row].size() - 1;
	}
	else if (col < 0)
		col = 0;

	//std::cout << "R: " << r << std::endl;
	//std::cout << "C: " << c << std::endl;

	//check collision
	std::bitset<4> wall(tileLayout[row][col]->getWall());
	//std::cout << "bit set for walls " << wall << std::endl;
	//std::cout << wall[3] << std::endl;
	//std::cout << wall[2] << std::endl;
	//std::cout << wall[1] << std::endl;
	//std::cout << wall[0] << std::endl;

	std::cout << "mapCoord:(" << row << ", " << col << "): " << tileLayout[row][col]->getWall() << std::endl;
	//check left wall
	if (wall[3]) {
		int left_bound = col * TILE_SIZE + WALL_SIZE;
		if (loc.getX() - PLAYER_RADIUS < left_bound) {
			printf("collided with left wall\n");
			loc.setX(left_bound + PLAYER_RADIUS);
		}
	}
	//check up wall
	if (wall[2]) {
		int up_bound = row * TILE_SIZE + WALL_SIZE;
		if (loc.getZ() - PLAYER_RADIUS < up_bound) {
			printf("collided with up wall\n");
			loc.setZ(up_bound + PLAYER_RADIUS);
		}
	}
	//check down wall
	if (wall[1]) {
		int down_bound = row * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc.getZ() + PLAYER_RADIUS > down_bound) {
			printf("collided with down wall\n");
			loc.setZ(down_bound - PLAYER_RADIUS);
		}
	}
	//check right wall
	if (wall[0]) {
		int right_bound = col * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc.getX() + PLAYER_RADIUS > right_bound) {
			printf("collided with right wall\n");
			loc.setX(right_bound - PLAYER_RADIUS);
		}
	}
}

ItemName Atlas::getTileItem(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return ItemName::EMPTY;

	return tileLayout[row][col]->getItem();
}

void Atlas::getItem(ItemName anItem, Item & outputItem)
{
	if (itemsMap.count(anItem) > 0)
	{
		outputItem = itemsMap.at(anItem);
	}
}

bool Atlas::hasGate(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	return tileLayout[row][col]->getTileType() == TileType::GATE;
}

bool Atlas::hasBox(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	if (tileLayout[row][col]->getTileType() == TileType::BOX)
	{
		return dynamic_cast<BoxTile*>(tileLayout[row][col])->hasBox();
	}

	return false;
}

bool Atlas::hasJail(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= jailLayout.size() || col >= jailLayout[row].size())
		return false;

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		return dynamic_cast<JailTile*>(tileLayout[row][col])->hasJail();
	}

	return false;
}

bool Atlas::isJailEmpty(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= jailEmptyLayout.size() || col >= jailEmptyLayout[row].size())
		return false;

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		return dynamic_cast<JailTile*>(tileLayout[row][col])->isJailEmpty();
	}

	return false;
}

void Atlas::placeInJail(Location & loc, int iter)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		dynamic_cast<JailTile*>(tileLayout[row][col])->placeAnimalInJail(iter);
	}
}

void Atlas::unlockJail(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		dynamic_cast<JailTile*>(tileLayout[row][col])->unlockJail();
	}
}

int Atlas::getJailProgress(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		dynamic_cast<JailTile*>(tileLayout[row][col])->getProgress();
	}
	return 0;
}

void Atlas::resetJail(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (tileLayout[row][col]->getTileType() == TileType::JAIL)
	{
		dynamic_cast<JailTile*>(tileLayout[row][col])->resetJail();
	}
}


void Atlas::updateBoxLayout(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return;

	if (tileLayout[row][col]->getTileType() == TileType::BOX)
	{
		dynamic_cast<BoxTile*>(tileLayout[row][col])->setBoxStatus(false);
	}

	if (keyLocations[row][col]) {
		tileLayout[row][col]->setItem(static_cast<ItemName>(keyLocations[row][col]));
	}
}

void Atlas::updateTileItem(Location & loc, ItemName anItem)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return;

	// If param is EMPTY, then an item has been picked up
	if (anItem == ItemName::EMPTY)
	{
		ItemName currItem = tileLayout[row][col]->getItem();
		if (itemsMap.count(currItem) > 0)
		{
			itemsMap.at(currItem).resetDropStatus();
		}
	}
	tileLayout[row][col]->setItem(anItem);
	
}

Tile * Atlas::getTileAt(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	return tileLayout[row][col];
}
bool Atlas::tileHasItem(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	return tileLayout[row][col]->getItem() != ItemName::EMPTY;
}

void Atlas::updateDroppedItem(ItemName anItem, Location loc)
{
	if (itemsMap.count(anItem) > 0)
	{
		Item & temp = itemsMap.at(anItem);
		int row, col;
		Atlas::getMapCoords(loc, row, col);
		temp.setDroppedIndices(row, col);
		if (temp.hasBeenMoved())
		{
			temp.setDropTime();
			//std::cout << "dropTime:" << clock() << std::endl;
		}
	}
}
void Atlas::checkDroppedItems()
{
	for (auto iter = itemsMap.begin(); iter != itemsMap.end(); iter++)
	{
		if (iter->second.hasBeenMoved())
		{
			//std::cout << "elapsedTime:" << iter->second.getDropDuration() << std::endl;
			if (iter->second.getDropDuration() > ITEM_DROP_DURATION)
			{
				int spawnRow, spawnCol;
				iter->second.getSpawnLocation(spawnRow, spawnCol);
				returnItemToSpawn(iter->first, spawnRow, spawnCol);
			}
		}
	}
}

// Recursive function to return items back to spawn if they cannot find an adjacent free tile
void Atlas::returnItemToSpawn(ItemName anItem, int spawnRow, int spawnCol)
{

	int dropRow, dropCol;
	itemsMap[anItem].getDropLocation(dropRow, dropCol);
	tileLayout[dropRow][dropCol]->setItem(ItemName::EMPTY);

	// Check if spawn tile location has an item
	if (tileLayout[spawnRow][spawnCol]->getItem() != ItemName::EMPTY)
	{
		ItemName occupyingItem = tileLayout[spawnRow][spawnCol]->getItem();

		int destRow, destCol;
		getAdjacentFreeTile(spawnRow, spawnCol, destRow, destCol);

		// If dest indices are not -1, then move the item, otherwise, it gets erased
		if (destRow != -1 && destCol != -1)
		{
			tileLayout[destRow][destCol]->setItem(occupyingItem);
			itemsMap[occupyingItem].setDroppedIndices(destRow, destCol);
		}
		else
		{
			int newSpawnRow, newSpawnCol;
			itemsMap[occupyingItem].getSpawnLocation(newSpawnRow, newSpawnCol);
			returnItemToSpawn(occupyingItem, newSpawnRow, newSpawnCol);
		}
	}
	tileLayout[spawnRow][spawnCol]->setItem(anItem);
	itemsMap[anItem].resetDropStatus();
}

void Atlas::getAdjacentFreeTile(int currRow, int currCol, int & destRow, int & destCol)
{
	int wallEncoding = tileLayout[currRow][currCol]->getWall();
	int radius = 1;
	std::vector < std::pair<int, int>> freeTiles;
	std::bitset<4> wall(wallEncoding);

	/*while (freeTiles.size() == 0)
	{*/

		//check left wall
		if (!wall[3]) {
			if (currCol - radius > 0)
			{
				if (tileLayout[currRow][currCol - radius]->getItem() == ItemName::EMPTY)
				{
					freeTiles.push_back(std::pair<int, int>(currRow, currCol - radius));
				}
			}
		}
		//check up wall
		if (!wall[2]) {
			if (currRow - radius > 0)
			{
				if (tileLayout[currRow - radius][currCol]->getItem() == ItemName::EMPTY)
				{
					freeTiles.push_back(std::pair<int, int>(currRow - radius, currCol ));
				}
			}
		}
		//check down wall
		if (!wall[1]) {
			if (currRow + radius > tileLayout.size())
			{
				if (tileLayout[currRow + radius][currCol]->getItem() == ItemName::EMPTY)
				{
					freeTiles.push_back(std::pair<int, int>(currRow + radius, currCol));
				}
			}
		}
		//check right wall
		if (!wall[0]) {
			if (currCol + radius < tileLayout[currRow].size())
			{
				if (tileLayout[currRow][currCol + radius]->getItem() == ItemName::EMPTY)
				{
					freeTiles.push_back(std::pair<int, int>(currRow, currCol + radius));
				}
			}
		}
	//}
		if (freeTiles.size() > 0)
		{
			auto p = freeTiles[rand() % freeTiles.size()];
			destRow = p.first;
			destCol = p.second;
		}
		else
		{
			destRow = -1;
			destCol = -1;
		}
}

std::string Atlas::encodeTileLayoutData()
{
	std::stringstream encodedData;
	for (int row = 0; row < tileLayout.size(); row++)
	{
		int tileCount = 0;
		for (int col = 0; col < tileLayout[row].size(); col++)
		{
			if (tileLayout[row][col]->isDirty())
			{
				encodedData << "tile: " << row << " " << col << "|";
				encodedData << "tileType: " << static_cast<int>(tileLayout[row][col]->getTileType()) << "|";
				encodedData << "tileData: " << tileLayout[row][col]->encodeTileData() << "|";
				tileCount++;
			}
		}

		if(tileCount > 0)
			encodedData << "newRow: break|";
	}
	encodedData << std::endl;
	return encodedData.str();

}
#include "Atlas.h"
#include <string>

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
	std::ifstream objectFile("../../maps/tinytinymap/table.txt");
	std::ifstream objectFile2("../../maps/tinytinymap/env_objs.txt");

	std::string wallLine;
	std::string boxLine;
	std::string gateLine;
	std::string heightLine;
	std::string jailLine;
	std::string rampLine;
	std::string keyDepositLine;
	std::string objectLine;
	std::string objectLine2;

	std::pair<int, int> tableLoc;

	std::getline(wallFile, wallLine); // removes first line from file
	std::getline(boxFile, boxLine); // removes first line from file
	std::getline(gateFile, gateLine); // removes first line from file
	std::getline(heightFile, heightLine); // removes first line from file
	std::getline(jailFile, jailLine); // removes first line from file
	std::getline(rampFile, rampLine); // removes first line from file
	std::getline(keyDepositFile, keyDepositLine); // removes first line from file
	std::getline(objectFile, objectLine); // removes first line from file
	std::getline(objectFile2, objectLine2); // removes first line from file
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
		std::getline(objectFile, objectLine);
		std::getline(objectFile2, objectLine2);
		std::stringstream wallStream(wallLine);
		std::stringstream boxStream(boxLine);
		std::stringstream gateStream(gateLine);
		std::stringstream heightStream(heightLine);
		std::stringstream jailStream(jailLine);
		std::stringstream rampStream(rampLine);
		std::stringstream keyDepositStream(keyDepositLine);
		std::stringstream objectStream(objectLine);
		std::stringstream objectStream2(objectLine2);
		std::string wallNum;
		std::string boxNum;
		std::string gateNum;
		std::string heightNum;
		std::string jailNum;
		std::string rampNum;
		std::string keyDepositNum;
		std::string objectNum;
		std::string objectNum2;
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
			objectStream >> objectNum;
			objectStream2 >> objectNum2;

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
			else if (objectNum != "0" || objectNum2 != "0")
			{
				type = TileType::OBJECT;
			}
			switch (type)
			{
			case TileType::BOX:
				tileRow.push_back(new BoxTile(wall, height));
				boxLocations.push_back(std::pair<int, int>(row, col));
				break;
			case TileType::JAIL:
				tileRow.push_back(new JailTile(wall, height));
				jailLocations.push_back(std::pair<int, int>(row, col));
				break;
			case TileType::GATE:
			{
				int num = std::stoi(gateNum);
				ItemModelType model = ItemModelType::apple;
				switch (num)
				{
				case 1: model = ItemModelType::door; break;
				case 2: model = ItemModelType::window; break;
				case 3: model = ItemModelType::vent; break;
					
				}
				GateTile * gateTile = new GateTile(std::stoi(gateNum), model, wall, height);
				tileRow.push_back(gateTile);
				gateMap.emplace(num, gateTile);
			}
				break;
			case TileType::RAMP:
				tileRow.push_back(new RampTile(static_cast<Orientation>(std::stoi(rampNum)), wall, height));
				break;
			case TileType::KEY_DROP: 
			{
				int num = std::stoi(keyDepositNum);
				Key key1 = static_cast<Key>(num * 1);
				Key key2 = static_cast<Key>(num * 2);
				Key key3 = static_cast<Key>(num * 3);
				ItemModelType model = ItemModelType::apple;
				switch (num)
				{
				case 1: model = ItemModelType::keyDropFrontExit; break;
				case 2: model = ItemModelType::keyDropBathroom; break;
				case 3: model = ItemModelType::keyDropVent; break;

				}
				tileRow.push_back(new KeyDropTile(std::vector<Key>({ key1, key2, key3 }), num, model, wall, height));
			}
				
				break;
			case TileType::OBJECT:
				if (objectNum == "1")
				{
					tileRow.push_back(new ObjectTile(ItemModelType::table, wall, height));
					tableLoc = std::pair<int, int>(row, col);
				}
				else if (objectNum2 != "0")
				{
					tileRow.push_back(new ObjectTile(static_cast<ItemModelType>(std::stoi(objectNum2)), wall, height));
				}
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
	while (boxLocations.size() > itemList.size())
	{
		int randPos = rand() % boxLocations.size();
		boxLocations.erase(boxLocations.begin() + randPos);
	}

	for(auto boxLocationsIter = boxLocations.begin(); boxLocationsIter != boxLocations.end(); boxLocationsIter++)
	{
		// Gets row and col location of the box
		int row = boxLocationsIter->first;
		int col = boxLocationsIter->second;
		ItemModelType randItem = ItemModelType::EMPTY;
		
		// Get random item that is still available
		/*while(std::find(itemList.begin(), itemList.end(), randItem) == itemList.end())
			randItem = static_cast<ItemModelType>(rand() % MAX_ITEMS + 1);*/
		randItem = itemList[rand() % itemList.size()];

		itemsMap.emplace(randItem, Item(randItem, row, col));
		keyLocations[row][col] = static_cast<int>(randItem);

		// Removes the item from the item list
		itemList.erase(std::find(itemList.begin(), itemList.end(), randItem));
	}

	itemsMap.emplace(ItemModelType::cake, Item(ItemModelType::cake, tableLoc.first, tableLoc.second)); // adds cake to table location
	tileLayout[tableLoc.first][tableLoc.second]->setItem(ItemModelType::cake);
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

void Atlas::detectWallCollision(Location & loc) {
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

	//check collision
	std::bitset<4> wall(tileLayout[row][col]->getWall());

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
void Atlas::detectObjectCollision(Location & loc) {
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

	if (row - 1 >= 0)
	{
		if ((tileLayout[row - 1][col]->getTileType() == TileType::OBJECT &&
			dynamic_cast<ObjectTile*>(tileLayout[row - 1][col])->getModel() != ItemModelType::painting) || tileLayout[row - 1][col]->getTileType() == TileType::JAIL)
		{
			int up_bound = row * TILE_SIZE;
			if (loc.getZ() - PLAYER_RADIUS <= up_bound) {
				printf("collided with up obj\n");
				loc.setZ(up_bound + PLAYER_RADIUS);
			}
		}
	}

	if (row + 1 < tileLayout.size())

	{
		if ((tileLayout[row + 1][col]->getTileType() == TileType::OBJECT &&
			dynamic_cast<ObjectTile*>(tileLayout[row + 1][col])->getModel() != ItemModelType::painting) || tileLayout[row + 1][col]->getTileType() == TileType::JAIL)
		{
			int down_bound = row * TILE_SIZE + TILE_SIZE-1;// need -1 so that it does not go into the next tile
			if (loc.getZ() + PLAYER_RADIUS >= down_bound) {
				printf("collided with down obj\n");
				loc.setZ(down_bound - PLAYER_RADIUS);
			}
		}
	}
	
	if (col - 1 >= 0)
	{
		if ((tileLayout[row][col - 1]->getTileType() == TileType::OBJECT &&
			dynamic_cast<ObjectTile*>(tileLayout[row][col - 1])->getModel() != ItemModelType::painting) || tileLayout[row][col - 1]->getTileType() == TileType::JAIL)
		{
			int left_bound = col * TILE_SIZE;
			if (loc.getX() - PLAYER_RADIUS <= left_bound) {
				printf("collided with left obj\n");
				loc.setX(left_bound + PLAYER_RADIUS);
			}
		}
			
		
	}

	if (col + 1 < tileLayout[row].size())
	{
		if ((tileLayout[row][col + 1]->getTileType() == TileType::OBJECT &&
			dynamic_cast<ObjectTile*>(tileLayout[row][col + 1])->getModel() != ItemModelType::painting) || tileLayout[row][col + 1]->getTileType() == TileType::JAIL)
		{
			int right_bound = col * TILE_SIZE + TILE_SIZE-1; // need -1 so that it does not go into the next tile
			if (loc.getX() + PLAYER_RADIUS >= right_bound) {
				printf("collided with right obj\n");
				loc.setX(right_bound - PLAYER_RADIUS);
			}
		}
	}

}

ItemModelType Atlas::getTileItem(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return ItemModelType::EMPTY;

	return tileLayout[row][col]->getItem();
}

void Atlas::getItem(ItemModelType anItem, Item & outputItem)
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

bool Atlas::hasHide(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	return tileLayout[row][col]->getTileType() == TileType::HIDE;
}

bool Atlas::hasJail(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	return tileLayout[row][col]->getTileType() == TileType::JAIL;
}

bool Atlas::isJailEmpty(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
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
		tileLayout[row][col]->setItem(static_cast<ItemModelType>(keyLocations[row][col]));
	}
	else // Randomly spawn powerups from boxes if it isnt a key box
	{
		int powerups = 5;
		int randNum = rand() % (powerups * POWERUP_DROP_CHANCE);
		switch (randNum)
		{
		case 0: tileLayout[row][col]->setItem(ItemModelType::apple);
			break;
		case 1: tileLayout[row][col]->setItem(ItemModelType::orange);
			break;
		case 2: tileLayout[row][col]->setItem(ItemModelType::bananaGreen);
			break;
		case 3: tileLayout[row][col]->setItem(ItemModelType::bananaPerfect);
			break;
		case 4: tileLayout[row][col]->setItem(ItemModelType::bananaVeryRipe);
			break;
		}
	}
}

void Atlas::updateTileItem(Location & loc, ItemModelType anItem)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return;

	// If param is EMPTY, then an item has been picked up
	if (anItem == ItemModelType::EMPTY)
	{
		ItemModelType currItem = tileLayout[row][col]->getItem();
		if (itemsMap.count(currItem) > 0)
		{
			itemsMap.at(currItem).resetDropStatus();
		}
	}
	tileLayout[row][col]->setItem(anItem);
	
}

void Atlas::updateGateProgress(int gateNum)
{
	gateMap.at(gateNum)->updateKeyProgress();
}
Tile * Atlas::getTileAt(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= 0 && row < tileLayout.size() && col >= 0 && col < tileLayout[row].size())
		return tileLayout[row][col];
	else
		return nullptr;
}
bool Atlas::tileHasItem(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	return tileLayout[row][col]->getItem() != ItemModelType::EMPTY;
}

void Atlas::updateDroppedItem(ItemModelType anItem, Location loc)
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
void Atlas::returnItemToSpawn(ItemModelType anItem, int spawnRow, int spawnCol)
{

	int dropRow, dropCol;
	itemsMap[anItem].getDropLocation(dropRow, dropCol);
	tileLayout[dropRow][dropCol]->setItem(ItemModelType::EMPTY);

	// Check if spawn tile location has an item
	if (tileLayout[spawnRow][spawnCol]->getItem() != ItemModelType::EMPTY)
	{
		ItemModelType occupyingItem = tileLayout[spawnRow][spawnCol]->getItem();

		int destRow, destCol;
		getAdjacentFreeTile(spawnRow, spawnCol, destRow, destCol);

		// If dest indices are not -1, then move the item to adjacent tile
		// Otherwise, it gets put back to its spawn location
		if (destRow != -1 && destCol != -1)
		{
			tileLayout[destRow][destCol]->setItem(occupyingItem);
			if(itemsMap.count(occupyingItem) > 0)
				itemsMap[occupyingItem].setDroppedIndices(destRow, destCol);
		}
		else
		{
			int newSpawnRow, newSpawnCol;
			if (itemsMap.count(occupyingItem) > 0)
			{
				itemsMap[occupyingItem].getSpawnLocation(newSpawnRow, newSpawnCol);
				returnItemToSpawn(occupyingItem, newSpawnRow, newSpawnCol);
			}
		}
	}
	tileLayout[spawnRow][spawnCol]->setItem(anItem);
	itemsMap[anItem].resetDropStatus();
}

void Atlas::getAdjacentFreeTile(int currRow, int currCol, int & destRow, int & destCol)
{
	int wallEncoding = tileLayout[currRow][currCol]->getWall();
	int catchRadius = 1;
	std::vector < std::pair<int, int>> freeTiles;
	std::bitset<4> wall(wallEncoding);

	//check left wall
	if (!wall[3]) {
		if (currCol - catchRadius > 0)
		{
			if (tileLayout[currRow][currCol - catchRadius]->getItem() == ItemModelType::EMPTY)
			{
				freeTiles.push_back(std::pair<int, int>(currRow, currCol - catchRadius));
			}
		}
	}
	//check up wall
	if (!wall[2]) {
		if (currRow - catchRadius > 0)
		{
			if (tileLayout[currRow - catchRadius][currCol]->getItem() == ItemModelType::EMPTY)
			{
				freeTiles.push_back(std::pair<int, int>(currRow - catchRadius, currCol ));
			}
		}
	}
	//check down wall
	if (!wall[1]) {
		if (currRow + catchRadius > tileLayout.size())
		{
			if (tileLayout[currRow + catchRadius][currCol]->getItem() == ItemModelType::EMPTY)
			{
				freeTiles.push_back(std::pair<int, int>(currRow + catchRadius, currCol));
			}
		}
	}
	//check right wall
	if (!wall[0]) {
		if (currCol + catchRadius < tileLayout[currRow].size())
		{
			if (tileLayout[currRow][currCol + catchRadius]->getItem() == ItemModelType::EMPTY)
			{
				freeTiles.push_back(std::pair<int, int>(currRow, currCol + catchRadius));
			}
		}
	}

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

bool Atlas::hasRamp(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	return tileLayout[row][col]->getTileType() == TileType::RAMP;
}

std::string Atlas::encodeTileLayoutData(bool newPlayerInit)
{
	std::stringstream encodedData;
	for (int row = 0; row < tileLayout.size(); row++)
	{
		int tileCount = 0;
		for (int col = 0; col < tileLayout[row].size(); col++)
		{
			if (tileLayout[row][col]->isDirty() || newPlayerInit)
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
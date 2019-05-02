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
	std::ifstream wallFile("layout.txt");
	std::ifstream boxFile("box.txt");
	std::string wallLine;
	std::string boxLine;
	//printf("INITIALIZING WALLS!\n");

	std::getline(wallFile, wallLine); // removes first line from file
	std::getline(boxFile, boxLine); // removes first line from file
	int row = 0;
	while (std::getline(wallFile, wallLine))
	{
		int col = 0;
		std::getline(boxFile, boxLine);
		std::stringstream wallStream(wallLine);
		std::stringstream boxStream(boxLine);
		std::string wallNum;
		std::string boxNum;
		std::vector<Tile> tileRow;
		while (wallStream >> wallNum)
		{
			boxStream >> boxNum;
			TileType type(TileType::DEFAULT);
			bool boxStatus = false;
			int height = 0;
			if (boxNum == "1")
			{
				type = TileType::BOX;
				boxStatus = true;
				boxLocations.push_back(std::pair<int, int>(row, col));
			}
			tileRow.push_back(Tile(std::stoi(wallNum), type, boxStatus, height));
			col++;
		}
		tileLayout.push_back(tileRow);
		row++;
	}

	// Debug print out the wall layout
	for (int r = 0; r < tileLayout.size(); r++) {
		for (int c = 0; c < tileLayout[r].size(); c++) {

			std::cout << tileLayout[r][c].getWall() << " ";
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
		keyLayout[row][col] = static_cast<int>(randItem);

		// Removes the item from the item list
		itemList.erase(std::find(itemList.begin(), itemList.end(), randItem));
	}

	std::cout << "keys" << std::endl;
	// Debug print key layout
	for (auto v : keyLayout)
	{
		for (auto x : v)
		{
			std::cout << x << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "endconstructor\n";
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
	std::bitset<4> wall(tileLayout[row][col].getWall());
	//std::cout << "bit set for walls " << wall << std::endl;
	//std::cout << wall[3] << std::endl;
	//std::cout << wall[2] << std::endl;
	//std::cout << wall[1] << std::endl;
	//std::cout << wall[0] << std::endl;

	std::cout << "mapCoord:(" << row << ", " << col << "): " << tileLayout[row][col].getWall() << std::endl;
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

	return tileLayout[row][col].getItem();
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

	return tileLayout[row][col].getTileType() == TileType::GATE;
}

bool Atlas::hasBox(Location & loc)
{
	// find which tile player is in
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return false;

	return tileLayout[row][col].getTileType() == TileType::BOX && tileLayout[row][col].hasBox();
}

bool Atlas::hasJail(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= jailLayout.size() || c >= jailLayout[r].size())
		return false;

	return jailLayout[r][c] != 0;
}

bool Atlas::isJailEmpty(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= jailEmptyLayout.size() || c >= jailEmptyLayout[r].size())
		return false;

	return jailLayout[r][c] != 0;
}

void Atlas::placeInJail(std::vector<float> & loc) 
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailLayout[r][c] = 1;
}

void Atlas::removeFromJail(std::vector<float> & loc)
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailLayout[r][c] = 0;
}


void Atlas::updateBoxLayout(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return;

	tileLayout[row][col].setBoxStatus(false);

	if (keyLayout[row][col]) {
		tileLayout[row][col].setItem(static_cast<ItemName>(keyLayout[row][col]));
	}
}

void Atlas::updateTileItem(Location & loc, ItemName anItem)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	if (row >= tileLayout.size() || col >= tileLayout[row].size())
		return;

	if (anItem != ItemName::EMPTY)
	{
		if (itemsMap.count(anItem) > 0)
		{
			Item & temp = itemsMap.at(anItem);

			/*if (temp.wasDropped() || !temp.isHeld())
			{
				temp.setDropped(false);
				temp.setHoldStatus(true);
			}*/
			
		}
	}
	tileLayout[row][col].setItem(anItem);
	
}

Tile & Atlas::getTileAt(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	return tileLayout[row][col];
}
bool Atlas::tileHasItem(Location & loc)
{
	int row = (int)(loc.getZ() / TILE_SIZE);
	int col = (int)(loc.getX() / TILE_SIZE);

	return tileLayout[row][col].getItem() != ItemName::EMPTY;
}

void Atlas::updateDroppedItem(ItemName anItem, Location loc)
{
	if (itemsMap.count(anItem) > 0)
	{
		Item & temp = itemsMap.at(anItem);
		temp.setHoldStatus(false);
		int row, col;
		Atlas::getMapCoords(loc, row, col);
		if (temp.hasBeenMoved(row, col))
		{
			temp.setDropped(true);
			temp.setDroppedTime(clock());
			temp.setDroppedIndices(row, col);
			std::cout << "dropTime:" << clock() << std::endl;

		}
	}

}
void Atlas::checkDroppedItems()
{
	for (auto iter = itemsMap.begin(); iter != itemsMap.end(); iter++)
	{
		if (iter->second.wasDropped())
		{
			std::cout << "elapsedTime:" << clock() - iter->second.getDroppedTime() << std::endl;
			if (clock() - iter->second.getDroppedTime() > ITEM_DROP_DURATION)
			{
				int row, col;
				iter->second.getStartLocation(row, col);

				int dropRow, dropCol;
				iter->second.getDropLocation(dropRow, dropCol);
				tileLayout[dropRow][dropCol].setItem(ItemName::EMPTY);
				tileLayout[row][col].setItem(iter->first);
				iter->second.setDropped(false);
			}
		}
	}
}


std::string Atlas::encodeWallLayoutData()
{
	return encode2DVectorData(wallLayout);
}

std::string Atlas::encodeClientKeyLayoutData()
{
	return encode2DVectorData(clientKeyLayout);
}
std::string Atlas::encodeGateLayoutData()
{
	return encode2DVectorData(gateLayout);
}
std::string Atlas::encodeBoxLayoutData()
{
	return encode2DVectorData(boxLayout);
}

std::string Atlas::encode2DVectorData(std::vector<std::vector<int>> layout)
{
	std::stringstream encodedData;

	for (int r = 0; r < layout.size(); r++) {
		for (int c = 0; c < layout[r].size(); c++) {
			encodedData << layout[r][c];

			if (c < layout[r].size() - 1)
				encodedData << " ";
		}
		if (r < layout.size() - 1)
			encodedData << " | ";
	}
	encodedData << std::endl;
	return encodedData.str();
}

std::string Atlas::encodeTileLayoutData()
{
	std::stringstream encodedData;
	for (int row = 0; row < tileLayout.size(); row++)
	{
		int tileCount = 0;
		for (int col = 0; col < tileLayout[row].size(); col++)
		{
			if (tileLayout[row][col].isDirty())
			{
				encodedData << "tile: " << row << " " << col << "|";
				encodedData << "tileData: " << tileLayout[row][col].encodeTileData() << "|";
				tileCount++;
			}
			
			
			/*if(col < tileLayout[row].size() - 1)
				encodedData << ", ";*/
			
		}
		/*if (row < tileLayout.size() - 1)
			encodedData << " | ";*/
		if(tileCount > 0)
			encodedData << "newRow: break|";
	}
	encodedData << std::endl;
	return encodedData.str();

}
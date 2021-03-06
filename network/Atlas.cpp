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
	srand(time(NULL));
	std::ifstream boxFile("../../maps/tinytinymap/item_spawn.txt");
	std::ifstream wallFile("../../maps/tinytinymap/walls.txt");
	std::ifstream heightFile("../../maps/tinytinymap/heights.txt");
	std::ifstream gateFile("../../maps/tinytinymap/player_exit.txt");
	std::ifstream jailFile("../../maps/tinytinymap/player_trap.txt");
	std::ifstream rampFile("../../maps/tinytinymap/ramps.txt");
	std::ifstream keyDepositFile("../../maps/tinytinymap/key_deposit.txt");
	std::ifstream objectFile("../../maps/tinytinymap/table.txt");
	std::ifstream objectFile2("../../maps/tinytinymap/env_objs.txt");
	std::ifstream playerSpawnFile("../../maps/tinytinymap/player_spawn.txt");

	std::string wallLine;
	std::string boxLine;
	std::string gateLine;
	std::string heightLine;
	std::string jailLine;
	std::string rampLine;
	std::string keyDepositLine;
	std::string objectLine;
	std::string objectLine2;
	std::string playerSpawnLine;

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
	std::getline(playerSpawnFile, playerSpawnLine); // removes first line from file
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
		std::getline(playerSpawnFile, playerSpawnLine);
		std::stringstream wallStream(wallLine);
		std::stringstream boxStream(boxLine);
		std::stringstream gateStream(gateLine);
		std::stringstream heightStream(heightLine);
		std::stringstream jailStream(jailLine);
		std::stringstream rampStream(rampLine);
		std::stringstream keyDepositStream(keyDepositLine);
		std::stringstream objectStream(objectLine);
		std::stringstream objectStream2(objectLine2);
		std::stringstream playerSpawnStream(playerSpawnLine);
		std::string wallNum;
		std::string boxNum;
		std::string gateNum;
		std::string heightNum;
		std::string jailNum;
		std::string rampNum;
		std::string keyDepositNum;
		std::string objectNum;
		std::string objectNum2;
		std::string playerSpawnNum;
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
			playerSpawnStream >> playerSpawnNum;

			

			// Initialize default variables for a tile
			TileType type(TileType::DEFAULT);
			int wall = std::stoi(wallNum);
			int height = std::stoi(heightNum);

			if (playerSpawnNum != "0")
			{
				int y = height / 2 * TILE_HEIGHT;
				playerSpawnLocations.push_back(Location(TILE_SIZE/2 + TILE_SIZE * col, y, TILE_SIZE/2 + TILE_SIZE * row));
				 // std::cout << "player spawn: " << row << ", " << col << std::endl;
			}

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
			{
				BoxTile * boxTile = new BoxTile(wall, height);
				tileRow.push_back(boxTile);
				boxLocations.push_back(std::pair<int, int>(row, col));
				allBoxLocations.push_back(std::pair<int, int>(row, col));
				boxTiles.push_back(boxTile);
			}
				break;
			case TileType::JAIL:
				tileRow.push_back(new JailTile(wall, height));
				 // std::cout << "row " << row << " col " << col << std::endl;
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
				ItemModelType key1 = static_cast<ItemModelType>((num - 1) * 3 + 1);
				ItemModelType key2 = static_cast<ItemModelType>((num - 1) * 3 + 2);
				ItemModelType key3 = static_cast<ItemModelType>((num - 1) * 3 + 3);
				ItemModelType model = ItemModelType::apple;
				switch (num)
				{
				case 1: model = ItemModelType::keyDropFrontExit; break;
				case 2: model = ItemModelType::keyDropBathroom; break;
				case 3: model = ItemModelType::keyDropVent; break;

				}
				tileRow.push_back(new KeyDropTile(std::vector<ItemModelType>({ key1, key2, key3 }), num, model, wall, height));
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
					ObjectTile * ot = new ObjectTile(static_cast<ItemModelType>(std::stoi(objectNum2)), wall, height);
					if (static_cast<ItemModelType>(std::stoi(objectNum2)) == ItemModelType::painting) {
						ot->setBearBuff(true);
					}
					tileRow.push_back(ot);
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

			 // std::cout << tileLayout[r][c]->getWall() << " ";
		}
		 // std::cout << std::endl;
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
		dynamic_cast<BoxTile*>(tileLayout[row][col])->setKeyBox(true);

		// Removes the item from the item list
		itemList.erase(std::find(itemList.begin(), itemList.end(), randItem));
	}

	itemsMap.emplace(ItemModelType::cake, Item(ItemModelType::cake, tableLoc.first, tableLoc.second)); // adds cake to table location
	tileLayout[tableLoc.first][tableLoc.second]->setItem(ItemModelType::cake);
	 // std::cout << "keys" << std::endl;
	// Debug print key layout
	for (auto v : keyLocations)
	{
		for (auto x : v)
		{
			 // std::cout << x << " ";
		}
		 // std::cout << std::endl;
	}
	std::cout << "end atlas constructor\n";
}

Atlas::~Atlas()
{
	for (auto tileRow : tileLayout)
	{
		for (auto * tile : tileRow)
		{
			delete tile;
		}
	}
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

	 // std::cout << "mapCoord:(" << row << ", " << col << "): " << tileLayout[row][col]->getWall() << std::endl;
	//check left wall
	if (wall[3]) {
		int left_bound = col * TILE_SIZE + WALL_SIZE;
		if (loc.getX() - PLAYER_RADIUS < left_bound) {
			// printf("collided with left wall\n");
			loc.setX(left_bound + PLAYER_RADIUS);
		}
	}
	//check up wall
	if (wall[2]) {
		int up_bound = row * TILE_SIZE + WALL_SIZE;
		if (loc.getZ() - PLAYER_RADIUS < up_bound) {
			// printf("collided with up wall\n");
			loc.setZ(up_bound + PLAYER_RADIUS);
		}
	}
	//check down wall
	if (wall[1]) {
		int down_bound = row * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc.getZ() + PLAYER_RADIUS > down_bound) {
			// printf("collided with down wall\n");
			loc.setZ(down_bound - PLAYER_RADIUS);
		}
	}
	//check right wall
	if (wall[0]) {
		int right_bound = col * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc.getX() + PLAYER_RADIUS > right_bound) {
			// printf("collided with right wall\n");
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
				// printf("collided with up obj\n");
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
				// printf("collided with down obj\n");
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
				// printf("collided with left obj\n");
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
				// printf("collided with right obj\n");
				loc.setX(right_bound - PLAYER_RADIUS);
			}
		}
	}

}

bool Atlas::canMoveToDestination(Location currentLoc, Location & destinationLoc)
{
	int currRow = 0;
	int currCol = 0;
	int destRow = 0;
	int destCol = 0;


	getMapCoords(currentLoc, currRow, currCol);
	getMapCoords(destinationLoc, destRow, destCol);

	if (destRow < 0 || destRow >= tileLayout.size())
		return false;

	if (destCol < 0 || destCol >= tileLayout[destRow].size())
		return false;

	if (currRow >= tileLayout.size())
		currRow = tileLayout.size() - 1;
	else if (currRow < 0)
		currRow = 0;

	if (currCol >= tileLayout[currRow].size())
		currCol = tileLayout[currRow].size() - 1;
	else if (currCol < 0)
		currCol = 0;

	int rowDiff = destRow - currRow;
	int colDiff = destCol - currCol;

	// Destination is on a diff tile
	if (rowDiff != 0 || colDiff != 0)
	{
		Tile * destTile = tileLayout[destRow][destCol];
		bool moveX = true;
		bool moveZ = true;
		Tile * currTile = tileLayout[currRow][currCol];
		std::bitset<4> wall(currTile->getWall());
		

		if (rowDiff < 0)
		{
			if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL
				|| wall[2])
			{
				moveZ = false;
			}
		}
		else if (rowDiff > 0)
		{
			if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL
				|| wall[1])
			{
				moveZ = false;
			}
		}
		
		if (colDiff < 0)
		{
			if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL
				|| wall[3])
			{
				moveX = false;
			}
		}
		else if (colDiff > 0)
		{
			if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL
				|| wall[0])
			{
				moveX = false;
			}
		}

		if (!moveX && !moveZ)
		{
			moveX = moveZ = true;
			// At a corner of the current tile
			// Positive row diff means 
			if (rowDiff < 0 && currRow > 0)
			{
				destTile = tileLayout[currRow - 1][currCol];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveZ = false;
				}
			}
			else if (rowDiff > 0 && currRow < tileLayout.size() - 1)
			{
				destTile = tileLayout[currRow + 1][currCol];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveZ = false;
				}
			}

			if (colDiff < 0 && currCol > 0)
			{
				destTile = tileLayout[currRow][currCol-1];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveX = false;
				}
			}
			else if (colDiff > 0 && currCol < tileLayout[currRow].size() - 1)
			{
				destTile = tileLayout[currRow][currCol+1];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveX = false;
				}
			}
			if(!moveX && !moveZ)
				return false;
			else if (!moveX)
			{
				destinationLoc.setX(currentLoc.getX());
			}
			else if (!moveZ)
			{
				destinationLoc.setZ(currentLoc.getZ());
			}
			else if (moveX && moveZ) // cant allow both to be true b/c they were false to be able to enter this if case
			{
				// randomly select one direction to move in if moving into the corner
				int randNum = rand() % 2;
				switch (randNum)
				{
				case 0: destinationLoc.setX(currentLoc.getX()); break;
				case 1: destinationLoc.setZ(currentLoc.getZ()); break;
				}
			}

		}
		else if (!moveX)
		{
			destinationLoc.setX(currentLoc.getX());
		}
		else if (!moveZ)
		{
			destinationLoc.setZ(currentLoc.getZ());
		}
	}
	// diagonal cases for checking wall collisions
	if (rowDiff != 0 && colDiff != 0)
	{
		Tile * destTile = tileLayout[destRow][destCol];
		bool moveX = true;
		bool moveZ = true;
		std::bitset<4> destWall(destTile->getWall());
		if (rowDiff < 0)
		{
			if (destWall[1])
			{
				moveZ = false;
			}
		}
		else if (rowDiff > 0)
		{
			if (destWall[2])
			{
				moveZ = false;
			}
		}

		if (colDiff < 0)
		{
			if (destWall[0])
			{
				moveX = false;
			}
		}
		else if (colDiff > 0)
		{
			if (destWall[3])
			{
				moveX = false;
			}
		}

		if (!moveX && !moveZ)
		{
			// randomly select one direction to move in if moving into the corner
			int randNum = rand() % 2;
			switch (randNum)
			{
			case 0: destinationLoc.setX(currentLoc.getX()); break;
			case 1: destinationLoc.setZ(currentLoc.getZ()); break;
			}
		}
	}

	// Check within the same tile
	if (rowDiff == 0 && colDiff == 0)
	{
		Location locDiff = destinationLoc - currentLoc;
		Location destWithPlayerRadius;

		bool moveX = true;
		bool moveZ = true;
		
		Tile * currTile = tileLayout[currRow][currCol];
		std::bitset<4> wall(currTile->getWall());
		if (locDiff.getX() < 0)
		{
			destWithPlayerRadius = Location(destinationLoc.getX() - PLAYER_RADIUS, destinationLoc.getY(), destinationLoc.getZ());
			int destWithPlayerRadiusRow = 0;
			int destWithPlayerRadiusCol = 0;
			getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
			int rowDiff = currRow - destWithPlayerRadiusRow;
			int colDiff = currCol - destWithPlayerRadiusCol;

			// Check if in different tile
			if (rowDiff != 0 || colDiff != 0)
			{
				Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
				
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveX = false;
				}
			}
		}
		else if(locDiff.getX() > 0)
		{
			destWithPlayerRadius = Location(destinationLoc.getX() + PLAYER_RADIUS, destinationLoc.getY(), destinationLoc.getZ());
			int destWithPlayerRadiusRow = 0;
			int destWithPlayerRadiusCol = 0;
			getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
			int rowDiff = currRow - destWithPlayerRadiusRow;
			int colDiff = currCol - destWithPlayerRadiusCol;

			if (rowDiff != 0 || colDiff != 0)
			{
				Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveX = false;
				}
			}
		}

		if (locDiff.getZ() < 0)
		{
			destWithPlayerRadius = Location(destinationLoc.getX(), destinationLoc.getY(), destinationLoc.getZ() - PLAYER_RADIUS);
			int destWithPlayerRadiusRow = 0;
			int destWithPlayerRadiusCol = 0;
			getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
			int rowDiff = currRow - destWithPlayerRadiusRow;
			int colDiff = currCol - destWithPlayerRadiusCol;

			if (rowDiff != 0 || colDiff != 0)
			{
				Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveZ = false;
				}
			}
		}
		else if (locDiff.getZ() > 0)
		{
			destWithPlayerRadius = Location(destinationLoc.getX(), destinationLoc.getY(), destinationLoc.getZ() + PLAYER_RADIUS);
			int destWithPlayerRadiusRow = 0;
			int destWithPlayerRadiusCol = 0;
			getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
			int rowDiff = currRow - destWithPlayerRadiusRow;
			int colDiff = currCol - destWithPlayerRadiusCol;

			if (rowDiff != 0 || colDiff != 0)
			{
				Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
				if ((destTile->getTileType() == TileType::OBJECT && dynamic_cast<ObjectTile*>(destTile)->getModel() != ItemModelType::painting) || destTile->getTileType() == TileType::JAIL)
				{
					moveZ = false;
				}
			}
		}

		if (!moveX && !moveZ)
			return false;
		else if(!moveX)
		{
			destinationLoc.setX(currentLoc.getX());
		}
		else if (!moveZ)
		{
			destinationLoc.setZ(currentLoc.getZ());
		}

	}

	// Check for walls in current tile using player radius and wall width
	Location locDiff = destinationLoc - currentLoc;
	Location destWithPlayerRadius;

	bool moveX = true;
	bool moveZ = true;
	////check left wall
	//if (wall[3]) {
	//	int left_bound = col * TILE_SIZE + WALL_SIZE;
	//	if (loc.getX() - PLAYER_RADIUS < left_bound) {
	//		// printf("collided with left wall\n");
	//		loc.setX(left_bound + PLAYER_RADIUS);
	//	}
	//}
	////check up wall
	//if (wall[2]) {
	//	int up_bound = row * TILE_SIZE + WALL_SIZE;
	//	if (loc.getZ() - PLAYER_RADIUS < up_bound) {
	//		// printf("collided with up wall\n");
	//		loc.setZ(up_bound + PLAYER_RADIUS);
	//	}
	//}
	////check down wall
	//if (wall[1]) {
	//	int down_bound = row * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
	//	if (loc.getZ() + PLAYER_RADIUS > down_bound) {
	//		// printf("collided with down wall\n");
	//		loc.setZ(down_bound - PLAYER_RADIUS);
	//	}
	//}
	////check right wall
	//if (wall[0]) {
	//	int right_bound = col * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
	//	if (loc.getX() + PLAYER_RADIUS > right_bound) {
	//		// printf("collided with right wall\n");
	//		loc.setX(right_bound - PLAYER_RADIUS);
	//	}
	//}

	Tile * currTile = tileLayout[currRow][currCol];
	std::bitset<4> wall(currTile->getWall());
	if (locDiff.getX() < 0)
	{
		destWithPlayerRadius = Location(destinationLoc.getX() - PLAYER_RADIUS, destinationLoc.getY(), destinationLoc.getZ());
		int destWithPlayerRadiusRow = 0;
		int destWithPlayerRadiusCol = 0;
		getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
		int rowDiff = currRow - destWithPlayerRadiusRow;
		int colDiff = currCol - destWithPlayerRadiusCol;

		// Check if in different tile
		if (rowDiff != 0 || colDiff != 0)
		{
			Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];

			if (wall[3])
			{
				moveX = false;
			}
		}
		else if (wall[3])// Check for same tile for wall collision
		{
			int left_bound = currCol * TILE_SIZE + WALL_SIZE;
			if (destWithPlayerRadius.getX() < left_bound)
				moveX = false;
		}
	}
	else if (locDiff.getX() > 0)
	{
		destWithPlayerRadius = Location(destinationLoc.getX() + PLAYER_RADIUS, destinationLoc.getY(), destinationLoc.getZ());
		int destWithPlayerRadiusRow = 0;
		int destWithPlayerRadiusCol = 0;
		getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
		int rowDiff = currRow - destWithPlayerRadiusRow;
		int colDiff = currCol - destWithPlayerRadiusCol;

		if (rowDiff != 0 || colDiff != 0)
		{
			Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
			if (wall[0])
			{
				moveX = false;
			}
		}
		else if (wall[0])
		{
			int right_bound = currCol * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
			if (destWithPlayerRadius.getX() > right_bound)
				moveX = false;
		}
	}

	if (locDiff.getZ() < 0)
	{
		destWithPlayerRadius = Location(destinationLoc.getX(), destinationLoc.getY(), destinationLoc.getZ() - PLAYER_RADIUS);
		int destWithPlayerRadiusRow = 0;
		int destWithPlayerRadiusCol = 0;
		getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
		int rowDiff = currRow - destWithPlayerRadiusRow;
		int colDiff = currCol - destWithPlayerRadiusCol;

		if (rowDiff != 0 || colDiff != 0)
		{
			Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
			if (wall[2])
			{
				moveZ = false;
			}
		}
		else if (wall[2])
		{
			int up_bound = currRow * TILE_SIZE + WALL_SIZE;
			if (destWithPlayerRadius.getZ() < up_bound)
				moveZ = false;
		}
	}
	else if (locDiff.getZ() > 0)
	{
		destWithPlayerRadius = Location(destinationLoc.getX(), destinationLoc.getY(), destinationLoc.getZ() + PLAYER_RADIUS);
		int destWithPlayerRadiusRow = 0;
		int destWithPlayerRadiusCol = 0;
		getMapCoords(destWithPlayerRadius, destWithPlayerRadiusRow, destWithPlayerRadiusCol);
		int rowDiff = currRow - destWithPlayerRadiusRow;
		int colDiff = currCol - destWithPlayerRadiusCol;

		if (rowDiff != 0 || colDiff != 0)
		{
			Tile * destTile = tileLayout[destWithPlayerRadiusRow][destWithPlayerRadiusCol];
			if (wall[1])
			{
				moveZ = false;
			}
		}
		else if (wall[1])
		{
			int down_bound = currRow * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
			if (destWithPlayerRadius.getZ() > down_bound)
				moveZ = false;
		}
	}


	if (!moveX && !moveZ)
		return false;
	else if (!moveX)
	{
		destinationLoc.setX(currentLoc.getX());
	}
	else if (!moveZ)
	{
		destinationLoc.setZ(currentLoc.getZ());
	}

	// Check if you're inside a wall and push you out
	detectWallCollision(destinationLoc);

	return true;
}

// Wall check for catching animals
bool Atlas::hasWallInBetween(Location currentLoc, Location & destinationLoc)
{
	int currRow = 0;
	int currCol = 0;
	int destRow = 0;
	int destCol = 0;

	getMapCoords(currentLoc, currRow, currCol);
	getMapCoords(destinationLoc, destRow, destCol);

	if (destRow < 0 || destRow > tileLayout.size())
		return false;

	if (destCol < 0 || destCol > tileLayout[destRow].size())
		return false;

	if (currRow >= tileLayout.size())
		currRow = tileLayout.size() - 1;
	else if (currRow < 0)
		currRow = 0;

	if (currCol >= tileLayout[currRow].size())
		currCol = tileLayout[currRow].size() - 1;
	else if (currCol < 0)
		currCol = 0;

	int rowDiff = destRow - currRow;
	int colDiff = destCol - currCol;

	if (rowDiff != 0 && colDiff != 0) // diagonal cases
	{
		Tile * currtile = tileLayout[currRow][currCol];
		Tile * destTile = tileLayout[destRow][destCol];
		bool currXDir = true;
		bool currZDir = true;
		bool destXDir = true;
		bool destZDir = true;
		std::bitset<4> currWall(currtile->getWall());
		std::bitset<4> destWall(destTile->getWall());
		if (rowDiff < 0) // Check up wall
		{
			if (currWall[2])
				currZDir = false;
		}
		else if (rowDiff > 0) // check down wall
		{
			if (currWall[1])
				currZDir = false;
		}

		if (colDiff < 0) // check left wall
		{
			if (currWall[3])
				currXDir = false;

		}
		else if (colDiff > 0) // check right wall
		{
			if (currWall[0])
				currXDir = false;
		}

		if (currXDir || currZDir) // the corner has only 1 of 2 walls so destination tile has to be checked for walls can be reached
		{

			if (rowDiff < 0) // Check down wall
			{
				if (destWall[1])
					destZDir = false;
			}
			else if (rowDiff > 0) // check up wall
			{
				if (destWall[2])
					destZDir = false;
			}

			if (colDiff < 0) // check right wall
			{
				if (destWall[0])
					destXDir = false;

			}
			else if (colDiff > 0) // check left wall
			{
				if (destWall[3])
					destXDir = false;
			}


			if (currXDir && destZDir || currZDir && destXDir)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else if (rowDiff != 0 || colDiff != 0)
	{
		Tile * tile = tileLayout[currRow][currCol];
		bool xDir = true;
		bool zDir = true;
		std::bitset<4> wall(tile->getWall());
		////check left wall
		//if (wall[3]) {
		////check up wall
		//if (wall[2]) {
		////check down wall
		//if (wall[1]) {
		////check right wall
		//if (wall[0]) {

		if (rowDiff < 0) // Check up wall
		{
			if(wall[2])
				return false;
		}
		else if (rowDiff > 0) // check down wall
		{
			if (wall[1])
				return false;
		}

		if (colDiff < 0) // check left wall
		{
			if (wall[3])
				return false;

		}
		else if (colDiff > 0) // check right wall
		{
			if (wall[0])
				return false;
		}
		
		return true;
	}
	else // within same tile = no walls to check
	{
		return true;
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
		BoxTile * boxTile = dynamic_cast<BoxTile*>(tileLayout[row][col]);
		boxTile->setBoxStatus(false);
   		if (!boxTile->isKeyBox())
		{
			boxTile->setBoxRespawnTimestamp();
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
			// // std::cout << "dropTime:" << clock() << std::endl;
		}
	}
}
bool Atlas::checkDroppedItems()
{
	bool updateClient = false;
	for (auto iter = itemsMap.begin(); iter != itemsMap.end(); iter++)
	{
		if (iter->second.hasBeenMoved())
		{
			// // std::cout << "elapsedTime:" << iter->second.getDropDuration() << std::endl;
			if (iter->second.getDropDuration() > ITEM_DROP_DURATION)
			{
				int spawnRow, spawnCol;
				iter->second.getSpawnLocation(spawnRow, spawnCol);
				returnItemToSpawn(iter->first, spawnRow, spawnCol);
				updateClient = true;
			}
		}
	}

	return updateClient;
}

bool Atlas::isItemPowerUp(ItemModelType itemName)
{
	return itemsMap.count(itemName) == 0;
}
void Atlas::unlockAllGates()
{
	for (auto gatePair : gateMap)
	{
		gatePair.second->updateKeyProgress();
		gatePair.second->updateKeyProgress();
		gatePair.second->updateKeyProgress();
	}
}
void Atlas::openAllBoxes()
{
	for (auto locationPair: allBoxLocations)
	{
		Location boxLoc(locationPair.second * TILE_SIZE, 0, locationPair.first * TILE_SIZE);
		updateBoxLayout(boxLoc);
	}
}
bool Atlas::checkBoxRespawn()
{
	bool updateClient = false;
	for (BoxTile * boxTile : boxTiles)
	{
		if (!boxTile->isKeyBox() && !boxTile->hasBox() && boxTile->getItem() == ItemModelType::EMPTY)
		{
			if (boxTile->getBoxRespawnTime() >= TIME_TO_RESPAWN_BOX)
			{
				boxTile->setBoxStatus(true);
				updateClient = true;
			}
		}
	}
	return updateClient;
}
void Atlas::returnItemToSpawn(ItemModelType anItem)
{
	if (itemsMap.count(anItem) > 0)
	{
		Item & temp = itemsMap.at(anItem);
		int spawnRow, spawnCol;
		temp.getSpawnLocation(spawnRow, spawnCol);
		returnItemToSpawn(temp.getName(), spawnRow, spawnCol);
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

Location & Atlas::getPlayerSpawnLocation(int id)
{
	return playerSpawnLocations[id % playerSpawnLocations.size()];
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

void Atlas::removeWalls(int row, int col)
{
	tileLayout[row][col]->setWall(0);
}
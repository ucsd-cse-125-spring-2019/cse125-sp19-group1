#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <bitset> 
#include <string.h>
#include "Tile.h"
#include "BoxTile.h"
#include "JailTile.h"
#include "GateTile.h"
#include "KeyDropTile.h"
#include "RampTile.h"
#include "HideTile.h"
#include "ObjectTile.h"
#include "GameConfigs.h"

#include <map>
#include <ctime>

class Atlas
{
public:
	Atlas();
	~Atlas();

	static void getMapCoords(Location & loc, int & row, int & col);

	void detectWallCollision(Location & loc);
	void detectObjectCollision(Location & loc);
	ItemModelType getTileItem(Location & loc);
	bool hasGate(Location & loc);
	bool hasBox(Location & loc);
	
	bool hasJail(Location & loc);
	bool isJailEmpty(Location & loc);
	void placeInJail(Location & loc, int iter);
	void unlockJail(Location & loc);
	int getJailProgress(Location & loc);
	void resetJail(Location & loc);

	void updateBoxLayout(Location & loc);
	Tile * getTileAt(Location & loc);

	bool tileHasItem(Location & loc);
	void updateTileItem(Location & loc, ItemModelType anItem);
	void getAdjacentFreeTile(int currRow, int currCol, int & row, int & col);
	void returnItemToSpawn(ItemModelType anItem, int currRow, int currCol);
	void returnItemToSpawn(ItemModelType anItem);

	std::string encodeTileLayoutData(bool newPlayerInit);
	bool hasRamp(Location & loc);
	bool hasHide(Location & loc);

	std::vector<Location> playerSpawnLocations;
	std::vector<Item> itemLocations;
	std::vector<std::pair<int, int>> boxLocations;
	std::vector<std::pair<int, int>> allBoxLocations;
	std::vector<std::pair<int, int>> jailLocations;
	std::map<ItemModelType, Item> itemsMap;
	std::map <int, GateTile *> gateMap;

	Location & getPlayerSpawnLocation(int id);
	void getItem(ItemModelType anItem, Item & outputItem);

	bool isItemPowerUp(ItemModelType itemName);
	bool checkDroppedItems();
	bool checkBoxRespawn();
	void updateDroppedItem(ItemModelType anItem, Location loc);
	void updateGateProgress(int gateNum);
	void unlockAllGates();
	void openAllBoxes();

	std::vector<std::vector<Tile *>> getServerTileLayout() { return tileLayout; }

	std::vector<ItemModelType> itemList = 
	{
		ItemModelType::key1,
		ItemModelType::key2,
		ItemModelType::key3,
		ItemModelType::plunger,
		ItemModelType::rope,
		ItemModelType::toiletPaper,
		ItemModelType::screwdriver1,
		ItemModelType::screwdriver2,
		ItemModelType::screwdriver3,
		/*ItemModelType::apple,
		ItemModelType::orange,
		ItemModelType::bananaGreen,
		ItemModelType::bananaPerfect,
		ItemModelType::bananaVeryRipe,*/
		
	};

protected:

	std::vector<std::vector<int>> keyLocations;
	std::vector<std::vector<Tile *>> tileLayout;
	std::vector<BoxTile*> boxTiles;

};

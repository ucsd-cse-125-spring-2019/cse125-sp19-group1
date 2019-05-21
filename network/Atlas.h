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
#include "RampTile.h"
#include "HideTile.h"
#include <map>
#include <ctime>

#define TILE_SIZE 20
#define TILE_HEIGHT 10
#define PLAYER_RADIUS 2
#define WALL_SIZE 2
#define MAX_ITEMS 14
#define ITEM_DROP_DURATION 10

class Atlas
{
public:
	Atlas();
	~Atlas();

	static void getMapCoords(Location & loc, int & row, int & col);

	void detectCollision(Location & loc);
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

	std::string encodeTileLayoutData(bool newPlayerInit);
	bool hasRamp(Location & loc);
	bool hasHide(Location & loc);

	std::vector<Item> itemLocations;
	std::vector<std::pair<int, int>> boxLocations;
	std::map<ItemModelType, Item> itemsMap;

	void getItem(ItemModelType anItem, Item & outputItem);

	void checkDroppedItems();
	void updateDroppedItem(ItemModelType anItem, Location loc);

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
		ItemModelType::apple,
		ItemModelType::orange,
		ItemModelType::bananaGreen,
		ItemModelType::bananaPerfect,
		ItemModelType::bananaVeryRipe,
		
	};

protected:

	std::vector<std::vector<int>> keyLocations;
	std::vector<std::vector<Tile *>> tileLayout;

	std::vector<std::vector<int>> jailLayout =
	{ {0,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> jailEmptyLayout =
	{ {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> jailProgressLayout =
	{ {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

};

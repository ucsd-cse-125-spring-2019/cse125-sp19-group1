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
#include "ObjectTile.h"
#include <map>
#include <ctime>

#define TILE_SIZE 20
#define TILE_HEIGHT 10
#define PLAYER_RADIUS 2
#define WALL_SIZE 2
#define MAX_ITEMS 9
#define ITEM_DROP_DURATION 10

class Atlas
{
public:
	Atlas();
	~Atlas();

	static void getMapCoords(Location & loc, int & row, int & col);

	void detectWallCollision(Location & loc);
	void detectObjectCollision(Location & loc);
	ItemName getTileItem(Location & loc);
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
	void updateTileItem(Location & loc, ItemName anItem);
	void getAdjacentFreeTile(int currRow, int currCol, int & row, int & col);
	void returnItemToSpawn(ItemName anItem, int currRow, int currCol);

	std::string encodeTileLayoutData(bool newPlayerInit);
	bool hasRamp(Location & loc);
	bool hasHide(Location & loc);

	std::vector<Item> itemLocations;
	std::vector<std::pair<int, int>> boxLocations;
	std::map<ItemName, Item> itemsMap;

	void getItem(ItemName anItem, Item & outputItem);

	void checkDroppedItems();
	void updateDroppedItem(ItemName anItem, Location loc);

	std::vector<ItemName> itemList = 
	{
		ItemName::KEY1,
		ItemName::KEY2,
		ItemName::KEY3,
		ItemName::KEY4,
		ItemName::KEY5,
		ItemName::KEY6,
		ItemName::KEY7,
		ItemName::KEY8,
		ItemName::KEY9
	};

protected:

	std::vector<std::vector<int>> keyLocations;
	std::vector<std::vector<Tile *>> tileLayout;

};

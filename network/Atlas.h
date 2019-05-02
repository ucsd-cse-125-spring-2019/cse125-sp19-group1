#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <bitset> 
#include <string.h>
#include "Tile.h"

#define TILE_SIZE 20
#define PLAYER_RADIUS 2
#define WALL_SIZE 2

class Atlas
{
public:
	Atlas();
	~Atlas();

	static void getMapCoords(Location & loc, int & row, int & col);

	void detectCollision(Location & loc);
	ItemName getTileItem(Location & loc);
	bool hasGate(Location & loc);
	bool hasBox(Location & loc);
	
	bool hasJail(std::vector<float> & loc);
	bool isJailEmpty(std::vector<float> & loc);
	void placeInJail(std::vector<float> & loc);
	void removeFromJail(std::vector<float> & loc);

	void updateBoxLayout(Location & loc);
	Tile & getTileAt(Location & loc);

	bool tileHasItem(Location & loc);
	void updateTileItem(Location & loc, ItemName anItem);

	std::string encodeTileLayoutData();
	std::string encodeWallLayoutData();
	std::string encodeClientKeyLayoutData();
	std::string encodeGateLayoutData();
	std::string encodeBoxLayoutData();
	std::string encode2DVectorData(std::vector<std::vector<int>> layout);

protected:
	std::vector<std::vector<int>> wallLayout =
	{ {12,4,4,4,4,4,5},
	  {8,0,0,0,0,0,1},
	  {8,0,0,0,0,0,1},
	  {8,0,0,0,0,0,1},
	  {8,0,0,0,0,0,1},
	  {8,0,0,0,0,0,1},
	  {10,2,2,2,2,2,3} };

	std::vector<std::vector<int>> keyLayout =
	{ {0,0,0,0,0,0,9},
	  {0,0,0,0,0,0,8},
	  {0,0,0,0,0,0,7},
	  {0,0,0,0,0,0,6},
	  {0,0,0,0,0,0,5},
	  {0,0,0,0,0,0,4},
	  {1,2,2,2,2,2,3} };

	std::vector<std::vector<int>> clientKeyLayout =
	{ {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> gateLayout =
	{ {1,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> boxLayout =
	{ {0,0,0,0,0,0,1},
	  {0,0,0,0,0,0,1},
	  {0,0,0,0,0,0,1},
	  {0,0,0,0,0,0,1},
	  {0,0,0,0,0,0,1},
	  {0,0,0,0,0,0,1},
	  {1,1,1,1,1,1,1} };

	std::vector<std::vector<Tile>> tileLayout;

	std::vector<std::vector<int>> jailLayout =
	{ {0,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
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

};


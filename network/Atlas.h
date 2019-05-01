#pragma once
#include <vector>
#include "Tile.h"

#define TILE_SIZE 20
#define PLAYER_RADIUS 2
#define WALL_SIZE 2

class Atlas
{
public:
	Atlas();
	~Atlas();

	static void getMapCoords(std::vector<float> & loc, int * row, int * col);

	void detectCollision(std::vector<float> & loc);
	int hasKey(std::vector<float> & loc);
	bool hasGate(std::vector<float> & loc);
	bool hasBox(std::vector<float> & loc);

	void updateBoxLayout(std::vector<float> & loc);

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
};


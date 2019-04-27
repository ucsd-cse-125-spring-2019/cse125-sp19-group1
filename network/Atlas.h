#pragma once
#include <vector>

class Atlas
{
public:
	Atlas();
	~Atlas();

	void detectCollision(std::vector<float> & loc);
	int hasKey(std::vector<float> & loc);
	bool hasGate(std::vector<float> & loc);
	//Hardcode map for now 
	const static int TILE_SIZE = 20;
	const static int PLAYER_RADIUS = 2;
	const static int WALL_SIZE = 2;

	

	std::string encodeWallData();

protected:
	std::vector<std::vector<int>> wallLayout =
	{ {12,4,5},
	  {8,0,1},
	  {8,0,1},
	  {8,0,1},
	  {8,0,1},
	  {8,0,1},
	  {10,2,3} };

	std::vector<std::vector<int>> keyLayout =
	{ {0,0,9},
	  {0,0,8},
	  {0,0,7},
	  {0,0,6},
	  {0,0,5},
	  {0,0,4},
	  {1,2,3} };

	std::vector<std::vector<int>> gateLayout =
	{ {1,0,0},
	  {0,0,0},
	  {0,0,0},
	  {0,0,0},
	  {0,0,0},
	  {0,0,0},
	  {0,0,0} };
};


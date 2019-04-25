#pragma once
#include <vector>

class Atlas
{
public:
	Atlas();
	~Atlas();

	void detectCollision(std::vector<int> loc);

	//Hardcode map for now 
	const static int TILE_SIZE = 20;
	const static int PLAYER_RADIUS = 2;
	const static int WALL_SIZE = 2;

	int layout[3][3] =
	{ {12,2,5},
	  {8,0,1},
	  {10,2,3} };

	std::vector< std::vector<int> > wallMap;
	std::string encodeWallData();
protected:
};
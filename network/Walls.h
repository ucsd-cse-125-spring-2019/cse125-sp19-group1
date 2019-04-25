#pragma once
#include <vector>

class Walls
{
public:
	Walls();
	~Walls();

	void detectCollision(std::vector<float> loc);

	//Hardcode map for now 
	const static int TILE_SIZE = 20;
	const static int PLAYER_RADIUS = 2; 
	const static int WALL_SIZE = 2;

	int layout[3][3] =
				{ {12,2,5},
				  {8,0,1},
				  {10,2,3} };

	std::string encodeWallData();
};


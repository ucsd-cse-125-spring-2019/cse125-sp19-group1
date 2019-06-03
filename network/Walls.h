#pragma once
#include <vector>
#include <string>

class Walls
{
public:
	Walls();
	~Walls();

	void detectCollision(std::vector<float> & loc);

	//Hardcode map for now 
	const static int TILE_SIZE = 20;
	const static int PLAYER_RADIUS = 2; 
	const static int WALL_SIZE = 2;

	std::vector<std::vector<int>> wallLayout = 
				{ {12,4,5},
				  {8,0,1},
				  {8,0,1},
				  {8,0,1},
				  {8,0,1},
				  {8,0,1},
				  {10,2,3} };

	std::string encodeWallData();
};


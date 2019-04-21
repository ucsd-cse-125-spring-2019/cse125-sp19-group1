#pragma once
#include <vector>

class Walls
{
public:
	Walls();
	~Walls();

	void detectCollision(int loc[3]);

	//Hardcode map for now 
	int TILE_SIZE = 20;
	int PLAYER_RADIUS = 5; 
	int WALL_SIZE = 2;

	int layout[3][3] =
				{ {12,2,5},
				  {8,0,1},
				  {10,2,3} };

};


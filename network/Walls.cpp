#include "Walls.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <bitset> 

//1000 = left = 8
//0100 = up = 4
//0010 = down = 2
//0001 = right = 1
//0000 = no wall = 0 

Walls::Walls()
{
	//Reading from a file to generate map
	/*std::ifstream infile("layout.txt");
	std::string line;
	printf("INITIALIZING WALLS!\n");	
	std::cout << infile.good() << std::endl;

	while (std::getline(infile, line))
	{
		std::cout << "reading the file line by line" << std::endl;
		std::cout << line << std::endl;
	}*/

	//Hardcode layout for now
	int c;
	int r;
	for (c = 0; c < 3; c++) {
		for (r = 0; r < 3; r++) {
			std::cout << layout[c][r] << " ";
		}
		std::cout << std::endl;
	}
}

void Walls::detectCollision(int loc[3]) {
	// find which tile player is in
	int r = (int)(loc[0] / TILE_SIZE);
	int c = (int)(loc[2] / TILE_SIZE);

	//check collision
	std::bitset<4> wall(layout[r][c]);
	std::cout << "bit set for walls " << wall << std::endl;

	//check left wall
	if (wall[0]) {
		int left_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			loc[0] = left_bound;
		}
	}
	//check up wall
	if (wall[1]) {
		int up_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[1] - PLAYER_RADIUS < up_bound) {
			
		}
	}
	//check down wall
	if (wall[2]) {
		int left_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			
		}
	}
	//check right wall
	if (wall[3]) {
		int left_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			
		}
	}


}




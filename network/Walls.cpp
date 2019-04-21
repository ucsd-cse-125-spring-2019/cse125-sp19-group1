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

	std::cout << "R: " << r << std::endl;
	std::cout << "C: " << c << std::endl;

	//check collision
	std::bitset<4> wall(layout[r][c]);
	std::cout << "bit set for walls " << wall << std::endl;

	std::cout << wall[3] << std::endl;
	std::cout << wall[2] << std::endl;
	std::cout << wall[1] << std::endl;
	std::cout << wall[0] << std::endl;

	//check left wall
	if (wall[3]) {
		printf("checking left wall\n");
		int left_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			loc[0] = left_bound+PLAYER_RADIUS;
		}
	}
	//check up wall
	if (wall[2]) {
		printf("checking up wall\n");
		int up_bound = c * TILE_SIZE + WALL_SIZE;
		if (loc[2] - PLAYER_RADIUS < up_bound) {
			loc[2] = up_bound+PLAYER_RADIUS;
		}
	}
	//check down wall
	if (wall[1]) {
		printf("checking down wall\n");
		int down_bound = c * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[2] + PLAYER_RADIUS > down_bound) {
			loc[2] = down_bound-PLAYER_RADIUS;
		}
	}
	//check right wall
	if (wall[0]) {
		printf("checking right wall\n");
		int right_bound = r * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[0] + PLAYER_RADIUS > right_bound) {
			loc[0] = right_bound-PLAYER_RADIUS;
		}
	}

	int i;
	for (i = 0; i < 3; i++) {
		std::cout << loc[i] << " ";
	}
	std::cout << std::endl;

}




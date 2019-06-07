#include "Walls.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <bitset> 
#include <string.h>

//1000 = left = 8
//0100 = up = 4
//0010 = down = 2
//0001 = right = 1
//0000 = no wall = 0 

Walls::Walls()
{
	//Reading from a file to generate map
	std::ifstream infile("layout.txt");
	char errmsg[100];
	strerror_s(errmsg, 100, errno);
	 // std::cout << errmsg << std::endl;
	std::string line;
	// printf("INITIALIZING WALLS!\n");	
	//layout.clear();
	// // std::cout << infile.good() << std::endl;
	int cols = 3;
	while (std::getline(infile, line))
	{
		// // std::cout << "reading the file line by line" << std::endl;
		// // std::cout << line << std::endl;

		std::stringstream lineStream(line);
		std::string num;
		std::vector<int> row;
		while (lineStream >> num)
		{
			row.push_back(std::stoi(num));
		}
		cols = row.size();
		//layout.push_back(row);
	}

	//Hardcode layout for now
	for (int r = 0; r < wallLayout.size(); r++) {
		for (int c = 0; c < wallLayout[r].size(); c++) {

			 // std::cout << wallLayout[r][c] << " ";
		}
		 // std::cout << std::endl;
	}
}

void Walls::detectCollision(std::vector<float> & loc) {
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= wallLayout.size())
		r = wallLayout.size()-1;

	if (c >= wallLayout[r].size())
	{
		c = wallLayout[r].size()-1;
	}

	// // std::cout << "R: " << r << std::endl;
	// // std::cout << "C: " << c << std::endl;

	//check collision
	std::bitset<4> wall(wallLayout[r][c]);
	// // std::cout << "bit set for walls " << wall << std::endl;

	// // std::cout << wall[3] << std::endl;
	// // std::cout << wall[2] << std::endl;
	// // std::cout << wall[1] << std::endl;
	// // std::cout << wall[0] << std::endl;

	 // std::cout << "layout(" << r << ", " << c << "): " << wallLayout[r][c] << std::endl;
	//check left wall
	if (wall[3]) {
		int left_bound = c * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			// printf("collided with left wall\n");
			// // std::cout << left_bound << "\tx: " << loc[0] << std::endl;
			loc[0] = left_bound+PLAYER_RADIUS;
		}
	}
	//check up wall
	if (wall[2]) {
		int up_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[2] - PLAYER_RADIUS < up_bound) {
			// printf("collided with up wall\n");
			// // std::cout << up_bound << "\tz: " << loc[2] << std::endl;
			loc[2] = up_bound+PLAYER_RADIUS;
		}
	}
	//check down wall
	if (wall[1]) {
		int down_bound = r * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[2] + PLAYER_RADIUS > down_bound) {
			// printf("collided with down wall\n");
			// // std::cout << down_bound << "\tz: " << loc[2] << std::endl;

			loc[2] = down_bound-PLAYER_RADIUS;
		}
	}
	//check right wall
	if (wall[0]) {
		int right_bound = c * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[0] + PLAYER_RADIUS > right_bound) {
			// printf("collided with right wall\n");
			// // std::cout << right_bound << "\tx: " << loc[0] << std::endl;
			loc[0] = right_bound-PLAYER_RADIUS;
		}
	}
}

std::string Walls::encodeWallData()
{
	std::stringstream encodedData;

	for (int r = 0; r < wallLayout.size(); r++) {
		for (int c = 0; c < wallLayout[r].size(); c++) {
			encodedData << wallLayout[r][c];

			if(c < wallLayout[r].size() - 1)
				encodedData << " ";
		}
		if(r < wallLayout.size() - 1)
			encodedData << " | ";
	}
	encodedData << std::endl;
	return encodedData.str();
}
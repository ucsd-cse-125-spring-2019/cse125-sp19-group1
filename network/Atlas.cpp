#include "Atlas.h"
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

void Atlas::getMapCoords(std::vector<float> & loc, int * row, int * col)
{
	*row = (int)(loc[2] / TILE_SIZE);
	*col = (int)(loc[0] / TILE_SIZE);
}


Atlas::Atlas()
{
	//Reading from a file to generate map
	std::ifstream infile("layout.txt");
	char errmsg[100];
	strerror_s(errmsg, 100, errno);
	std::cout << errmsg << std::endl;
	std::string line;
	printf("INITIALIZING WALLS!\n");
	//wallLayout.clear();
	//int cols = 3;
	//layout.clear();
	while (std::getline(infile, line))
	{
		std::stringstream lineStream(line);
		std::string num;
		std::vector<int> row;
		while (lineStream >> num)
		{
			row.push_back(std::stoi(num));
		}
		//cols = row.size();
		//wallLayout.push_back(row);
		//layout.push_back(row);
	}

	//Hardcode layout for now
	for (int r = 0; r < wallLayout.size(); r++) {
		for (int c = 0; c < wallLayout[r].size(); c++) {

			std::cout << wallLayout[r][c] << " ";
		}
		std::cout << std::endl;
	}
}

void Atlas::detectCollision(std::vector<float> & loc) {
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= wallLayout.size())
		r = wallLayout.size() - 1;

	if (c >= wallLayout[r].size())
	{
		c = wallLayout[r].size() - 1;
	}

	//std::cout << "R: " << r << std::endl;
	//std::cout << "C: " << c << std::endl;

	//check collision
	std::bitset<4> wall(wallLayout[r][c]);
	//std::cout << "bit set for walls " << wall << std::endl;
	//std::cout << wall[3] << std::endl;
	//std::cout << wall[2] << std::endl;
	//std::cout << wall[1] << std::endl;
	//std::cout << wall[0] << std::endl;

	std::cout << "layout(" << r << ", " << c << "): " << wallLayout[r][c] << std::endl;
	//check left wall
	if (wall[3]) {
		int left_bound = c * TILE_SIZE + WALL_SIZE;
		if (loc[0] - PLAYER_RADIUS < left_bound) {
			printf("collided with left wall\n");
			//std::cout << left_bound << "\tx: " << loc[0] << std::endl;
			loc[0] = left_bound + PLAYER_RADIUS;
		}
	}
	//check up wall
	if (wall[2]) {
		int up_bound = r * TILE_SIZE + WALL_SIZE;
		if (loc[2] - PLAYER_RADIUS < up_bound) {
			printf("collided with up wall\n");
			//std::cout << up_bound << "\tz: " << loc[2] << std::endl;
			loc[2] = up_bound + PLAYER_RADIUS;
		}
	}
	//check down wall
	if (wall[1]) {
		int down_bound = r * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[2] + PLAYER_RADIUS > down_bound) {
			printf("collided with down wall\n");
			//std::cout << down_bound << "\tz: " << loc[2] << std::endl;

			loc[2] = down_bound - PLAYER_RADIUS;
		}
	}
	//check right wall
	if (wall[0]) {
		int right_bound = c * TILE_SIZE + (TILE_SIZE - WALL_SIZE);
		if (loc[0] + PLAYER_RADIUS > right_bound) {
			printf("collided with right wall\n");
			//std::cout << right_bound << "\tx: " << loc[0] << std::endl;
			loc[0] = right_bound - PLAYER_RADIUS;
		}
	}
}

int Atlas::hasKey(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= clientKeyLayout.size() || c >= clientKeyLayout[r].size())
		return -1;

	// if check to remove key from map when picked up
	/*if (keyLayout[r][c])
	{
		keyLayout
	}*/
	return clientKeyLayout[r][c];
}

bool Atlas::hasGate(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= gateLayout.size() || c >= gateLayout[r].size())
		return false;

	return gateLayout[r][c] != 0;
}

bool Atlas::hasBox(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= boxLayout.size() || c >= boxLayout[r].size())
		return false;

	return boxLayout[r][c] != 0;
}

bool Atlas::hasJail(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= jailLayout.size() || c >= jailLayout[r].size())
		return false;

	return jailLayout[r][c] != 0;
}

bool Atlas::isJailEmpty(std::vector<float> & loc)
{
	// find which tile player is in
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	if (r >= jailEmptyLayout.size() || c >= jailEmptyLayout[r].size())
		return false;

	return jailLayout[r][c] != 0;
}

void Atlas::placeInJail(std::vector<float> & loc) 
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailLayout[r][c] = 1;
}

void Atlas::removeFromJail(std::vector<float> & loc)
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailLayout[r][c] = 0;
}


void Atlas::updateBoxLayout(std::vector<float> & loc) 
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);
	
	boxLayout[r][c] = 0;
	if (keyLayout[r][c]) {
		clientKeyLayout[r][c] = keyLayout[r][c];
	}
}

void Atlas::unlockJail(std::vector<float> & loc)
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailProgressLayout[r][c] += 1;
}

int Atlas::getJailProgress(std::vector<float> & loc)
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	return jailProgressLayout[r][c];
}

void Atlas::resetJail(std::vector<float> & loc)
{
	int r = (int)(loc[2] / TILE_SIZE);
	int c = (int)(loc[0] / TILE_SIZE);

	jailProgressLayout[r][c] = 0;
}


std::string Atlas::encodeWallLayoutData()
{
	return encode2DVectorData(wallLayout);
}

std::string Atlas::encodeClientKeyLayoutData()
{
	return encode2DVectorData(clientKeyLayout);
}
std::string Atlas::encodeGateLayoutData()
{
	return encode2DVectorData(gateLayout);
}
std::string Atlas::encodeBoxLayoutData()
{
	return encode2DVectorData(boxLayout);
}

std::string Atlas::encode2DVectorData(std::vector<std::vector<int>> layout)
{
	std::stringstream encodedData;

	for (int r = 0; r < layout.size(); r++) {
		for (int c = 0; c < layout[r].size(); c++) {
			encodedData << layout[r][c];

			if (c < layout[r].size() - 1)
				encodedData << " ";
		}
		if (r < layout.size() - 1)
			encodedData << " | ";
	}
	encodedData << std::endl;
	return encodedData.str();
}
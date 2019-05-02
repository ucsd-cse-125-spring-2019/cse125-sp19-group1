#pragma once
#include <vector>

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
	bool hasJail(std::vector<float> & loc);
	bool isJailEmpty(std::vector<float> & loc);
	void placeInJail(std::vector<float> & loc);
	void removeFromJail(std::vector<float> & loc);
	void unlockJail(std::vector<float> & loc);
	int getJailProgress(std::vector<float> & loc);
	void resetJail(std::vector<float> & loc);

	void updateBoxLayout(std::vector<float> & loc);
	//Hardcode map for now 
	/*const static int TILE_SIZE = 20;
	const static int PLAYER_RADIUS = 2;
	const static int WALL_SIZE = 2;
*/
	

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

	std::vector<std::vector<int>> jailLayout =
	{ {0,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {1,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> jailEmptyLayout =
	{ {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };

	std::vector<std::vector<int>> jailProgressLayout =
	{ {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0} };
};


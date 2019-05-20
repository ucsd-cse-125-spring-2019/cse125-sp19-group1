#include "GameManager.h"

GameManager::GameManager()
{
	gameData = new GameData(SERVER_GAMEDATA);
}

bool GameManager::gameHasStarted()
{
	return gameStarted;
}

bool GameManager::gameIsOver()
{
	return animalWin || chefWin;
}

void GameManager::updateRightEvent(int id)
{
	moveRight = true;
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX() + SPEED, loc.getY(), loc.getZ());
	//gameData->getPlayer(id)->setFacingDir(2);

	updatePlayerCollision(id, 0);
}

void GameManager::updateBackwardEvent(int id)
{
	moveBackward = true;
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() - SPEED);
	//gameData->getPlayer(id)->setFacingDir(3);

	updatePlayerCollision(id, 1);
}

void GameManager::updateForwardEvent(int id)
{
	moveForward = true;
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + SPEED);
	//gameData->getPlayer(id)->setFacingDir(1);
	updatePlayerCollision(id, 2);
}

void GameManager::updateLeftEvent(int id)
{
	moveLeft = true;
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX() - SPEED, loc.getY(), loc.getZ());
	//gameData->getPlayer(id)->setFacingDir(4);

	updatePlayerCollision(id, 3);
}

void GameManager::updateHeight(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	if (!gameData->getAtlas()->hasRamp(loc)) 
	{ 
		int y = gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 * TILE_HEIGHT;
		gameData->getPlayer(id)->setLocation(loc.getX(), y, loc.getZ());
		return; 
	}

	std::cout << "THERES A RAMP" << std::endl;

	int x = loc.getX();
	int y;
	int z = loc.getZ();

	RampTile * rampTile = (RampTile *)(gameData->getAtlas()->getTileAt(loc));

	if (rampTile->getRampDirection() == Orientation::NORTH)
	{
		y = (int) (TILE_SIZE - z % TILE_SIZE) * ((double) TILE_HEIGHT / TILE_SIZE) 
			* (gameData->getAtlas()->getTileAt(loc)->getHeight()/2 +1);
	}
	else if (rampTile->getRampDirection() == Orientation::SOUTH)
	{
		y = (int)(z % TILE_SIZE) * ((double) TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	else if (rampTile->getRampDirection() == Orientation::EAST)
	{
		y = (int)(x % TILE_SIZE) * ((double)TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	else if (rampTile->getRampDirection() == Orientation::WEST)
	{
		y = (int)(TILE_SIZE - x % TILE_SIZE) * ((double)TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	gameData->getPlayer(id)->setLocation(x, y, z);
}

void GameManager::updatePlayerCollision(int id, int dir) 
{
	Location pLoc = gameData->getPlayer(id)->getLocation();
	std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	
	//0 == right
	//1 == down
	//2 == forward
	//3 == left
	//dmap<string, vector<int>>::iterator it;

	std::map < int, Player * > players = gameData->getAllPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (it->first == id) {
			continue;
		}

		//calculate distance between two points 
		int my_x = loc[0];
		int my_z = loc[2];

		int ot_x = it->second->getLocation().getX();
		int ot_z = it->second->getLocation().getZ();

		float dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
		
		if (dist < 2 * PLAYER_RADIUS) 
		{
			if (dir == 0) 
			{
				loc[0] = ot_x - 2 * PLAYER_RADIUS;
			} 
			else if (dir == 1)
			{
				loc[2] = ot_z + 2 * PLAYER_RADIUS;
			}
			else if (dir == 2)
			{
				loc[2] = ot_z - 2 * PLAYER_RADIUS;
			}
			else
			{
				loc[0] = ot_x + 2 * PLAYER_RADIUS;
			}
			gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
		}
	}
}

void GameManager::updateCollision(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	//std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	gameData->getAtlas()->detectWallCollision(loc);
	//gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
	gameData->getPlayer(id)->setLocation(loc);
	gameData->getAtlas()->detectObjectCollision(loc);
	gameData->getPlayer(id)->setLocation(loc);


}
//
//void GameManager::resetGame() 
//{
//	std::cout << "CALLING RESET GAME" << std::endl;
//	gameData = new GameData(SERVER_GAMEDATA);
//
//	//reset players
//	int count = 0;
//	std::map<unsigned int, SOCKET>::iterator iter;
//	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
//	{
//		Location loc = gameData->initLocs[count];
//		gameData->addNewPlayer(iter->first, loc, ClientType::SERVER_SIDE);
//		count++;
//	}
//	std::cout << "GAME RESET" << std::endl;
//
//}
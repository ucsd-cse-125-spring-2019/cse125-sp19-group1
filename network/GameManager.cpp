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

void GameManager::updatePlayerModel(int id, ModelType model)
{
	gameData->getPlayer(id)->setModelType(model);
}
void GameManager::updateMovement(int dir, int id)
{
	if (gameData->getPlayer(id)->getInteracting() ||
		gameData->getPlayer(id)->getIsCaught() ||
		gameData->getPlayer(id)->getHidden()) {
		return;
	}
	switch (dir)
	{
	case 1: updateForwardEvent(id);
		break;
	case 2: updateBackwardEvent(id);
		break;
	case 3: updateLeftEvent(id);
		break;
	case 4: updateRightEvent(id);
		break;
	}
	updateCollision(id);
	updateHeight(id);
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

void GameManager::resetGame() 
{
	std::cout << "CALLING RESET GAME" << std::endl;
	chefWin = false;
	animalWin = false;
	//gameData = new GameData(SERVER_GAMEDATA);

	////reset players
	//int count = 0;
	//std::map<unsigned int, SOCKET>::iterator iter;
	//for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	//{
	//	Location loc = gameData->initLocs[count];
	//	gameData->addNewPlayer(iter->first, loc, ClientType::SERVER_SIDE);
	//	count++;
	//}
	//std::cout << "GAME RESET" << std::endl;

}

std::string GameManager::encodeGameData(bool newPlayerInit)
{
	return gameData->encodeGameData(newPlayerInit);
}

void GameManager::addNewPlayer(int anID, Location aLoc, ClientType type)
{
	gameData->addNewPlayer(anID, Location(30, 0, 30), ClientType::SERVER_SIDE);
}

void GameManager::removePlayer(int anID, ClientType type)
{
	gameData->removePlayer(anID, ClientType::SERVER_SIDE);
}

void GameManager::resetPlayerDirectionFlags()
{
	moveForward = moveBackward = moveLeft = moveRight = false;
}

void GameManager::updatePlayerFacingDirection(int id)
{
	if (Player * player = gameData->getPlayer(id))
	{
		int sum = 0;
		if (moveForward)
		{
			sum += 5;
		}
		if (moveBackward)
		{
			sum -= 5;
		}
		if (moveRight)
		{
			sum += 1;
		}
		if (moveLeft)
		{
			sum -= 1;
		}

		if (sum != 0)
		{
			Direction facingDirection = static_cast<Direction>(sum);
			player->setFacingDirection(facingDirection);
		}
	}
}

bool GameManager::isCountingDown()
{
	if (gameData->countdownDone())
	{
		gameStarted = true;
	}
	return gameData->countdownStarted() && !gameData->countdownDone();
}

void GameManager::update()
{
	gameData->getAtlas()->checkDroppedItems();


	allPlayersReady = true;
	for (auto iter = gameData->getAllPlayers().begin(); iter != gameData->getAllPlayers().end(); iter++)
	{
		if (Player * player = iter->second)
		{
			if (!player->isReady())
			{
				allPlayersReady = false;
			}
			Location loc = player->getLocation();

			if (player->getIsCaught())
			{
				continue;
			}

			if (player->getInteracting())
			{
				double seconds = player->getInteractingTime(0);
				if (player->isChef())
				{
					if (seconds > gameData->getChefSwingTime()) {
						std::cout << "CAN SWING AGAIN" << std::endl;
						player->setInteracting(false);
					}
				}
				else
				{
					if (seconds > gameData->getBoxTime()) {
						std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
						gameData->getAtlas()->updateBoxLayout(loc);
						player->setInteracting(false);
					}
				}
			}

			if (player->getOpenJail())
			{
				double seconds = player->getInteractingTime(1);
				if (!player->isChef())
				{
					if (seconds > gameData->getOpenJailTime()) {
						player->setOpenJail(false);
					}
				}
			}

			if (gameData->getAtlas()->hasGate(loc))
			{
				GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));

				if (!gateTile->isOpen())
				{
					double seconds = player->getInteractingTime(0);

					if (player->getOpeningGate() && gateTile->getCurrentConstructTime() + seconds >= TOTAL_CONSTRUCT_TIME)
					{
						std::cout << "GATE CONSTRUCTED" << std::endl;
						gateTile->constructGate(seconds);
					}
				}
			}
		}
	}
}

void GameManager::interactEvent(int id)
{

	if (Player * player = gameData->getPlayer(id))
	{
		if (player->getInteracting() ||
			player->getIsCaught() ||
			player->getHidden()) {
			return;
		}

		if (player->isChef())
		{
			swingAction(player);
		}
		else
		{
			Location loc = player->getLocation();
			ItemName item = gameData->getAtlas()->getTileItem(loc);
			Direction dir = player->getFacingDirection();
			ObjectTile * objectTile = gameData->getAdjacentObjectTile(loc, dir);

			if (player->getInventory() == ItemName::EMPTY && (item != ItemName::EMPTY || objectTile && objectTile->getItem() == ItemName::CAKE))
			{

				if (objectTile && objectTile->getItem() == ItemName::CAKE)
				{
					std::cout << "table has cake!" << std::endl;
					player->setInventory(objectTile->getItem());
					objectTile->setItem(ItemName::EMPTY);

				}
				else if (item != ItemName::EMPTY)
				{
					player->setInventory(item);
					gameData->getAtlas()->updateTileItem(loc, ItemName::EMPTY);
				}
			}
			else if (GateTile * gateTile = gameData->getGateTile(loc))
			{

				//GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
				if (gateTile->isValidKey(static_cast<Key>(player->getInventory())))
				{

					gateTile->updateKeyProgress(static_cast<Key>(player->getInventory()));
					player->setInventory(ItemName::EMPTY);
				}
				if (gateTile->hasAllKeys() && !player->getOpeningGate())
				{

					player->setOpeningGate(true);
					player->setActionStartTime();
				}
				if (gateTile->hasAllKeys() && gateTile->isOpen())
				{
					if (static_cast<Key>(player->getInventory()) == Key::CAKE)
					{
						animalWin = true;
						std::cout << "ANIMAL WIN" << std::endl;
					}

				}
			}
			//else if (gameData->getAtlas()->hasJail(loc))
			else if (JailTile * jailTile = gameData->getJailTile(loc))
			{
				//JailTile * jailTile = (JailTile *)(gameData->getAtlas()->getTileAt(loc));
				player->setOpenJail(true);
				player->setStartJailTime();

				//update jail progres
				std::cout << "UNLOCKING JAIL" << std::endl;

				jailTile->unlockJail();


				//check if jail progress == 5
				if (jailTile->getProgress() >= 5)
				{
					std::cout << "ANIMAL IS RELEASED" << std::endl;
					//update animal 
					int animal = jailTile->getCapturedAnimal();
					gameData->getPlayer(animal)->setIsCaught(false);

					//update jail
					jailTile->resetJail();

				}
			}
			else if (gameData->getAtlas()->hasBox(loc))
			{
				std::cout << "HAS BOX" << std::endl;
				if (!player->getInteracting()) {
					std::cout << "starting to interact!" << std::endl;
					player->setInteracting(true);
					player->setActionStartTime();
				}
			}
		}
		//}
	}
}

void GameManager::swingAction(Player * player)
{
	Location loc = player->getLocation();

	//drop off animal
	if (player->getCaughtAnimal() && gameData->getAtlas()->hasJail(loc) && (gameData->getAtlas()->isJailEmpty(loc)))
	{
		int animal = player->getCaughtAnimalId();
		//update jail with animal
		std::cout << "PLACE ANIMAL IN JAIL" << std::endl;
		gameData->getAtlas()->placeInJail(loc, animal);
		player->setCaughtAnimal(false);

		//update animal's location to jail
		int x = (int)(loc.getX() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
		int y = loc.getY();
		int z = (int)(loc.getZ() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
		gameData->getPlayer(animal)->setLocation(x, y, z);

	}
	else
	{
		std::cout << "SWINGING" << std::endl;
		if (!player->getInteracting())
		{
			std::cout << "CAN SWING" << std::endl;
			player->setInteracting(true);
			player->setActionStartTime();

			chefWin = true;
			for (auto iter2 = gameData->getAllPlayers().begin(); iter2 != gameData->getAllPlayers().end(); iter2++)
			{
				if (iter2->first == id)
				{
					continue;
				}

				if (!iter2->second->isChef() &&
					!iter2->second->getIsCaught())
				{
					chefWin = false;
				}

				Location tLoc = iter2->second->getLocation();
				if (player->inRange(loc, tLoc) &&
					!iter2->second->getIsCaught())
				{
					player->setCaughtAnimal(true);
					player->setCaughtAnimalId(iter2->first);
					iter2->second->setIsCaught(true);

					//update animal's location to chef's location
					iter2->second->setLocation(player->getLocation());
				}

				if (!iter2->second->getIsCaught())
				{
					chefWin = false;
				}
			}
		}
		if (chefWin)
		{
			std::cout << "CHEF WIN" << std::endl;
		}
	}
}

void GameManager::togglePlayerReady(int id)
{
	gameData->getPlayer(id)->toggleReady();
}
#include "ServerGame.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
 

ServerGame::ServerGame(void)
{
	initCharacters = false;
	gameStarted = true;
	newPlayerInit = false;
	allPlayersReady = false;
    // id's to assign clients for our table
    client_id = 1;
 
	bool chefWin = false;
	bool animalWin = false;
    // set up the server network to listen 
    network = new ServerNetwork(); 
	gameData = new GameData(SERVER_GAMEDATA);

}
ServerGame::~ServerGame()
{
	delete network;
	delete gameData;
}

 
void ServerGame::update() 
{

    // get new clients
    if(network->acceptNewClient(client_id))
    {
         printf("client %d has been connected to the server\n",client_id); 
    }
	receiveFromClients();
}

void ServerGame::receiveFromClients()
{

	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;


	for (iter = network->sessions.begin(); iter != network->sessions.end(); )//iter++)
	{
		int playerID = iter->first;
		int data_length = network->receiveData(iter->first, network_data);
		if (data_length <= 0)
		{
			//no data recieved
			if (network->sessions.size() == 0)
				break;

			if (data_length == 0 || (data_length == -1 && (WSAGetLastError() == CONNECTION_RESET_ERROR || WSAGetLastError() == CONNECTION_ABORT_ERROR)))
			{
				printf("Client disconnected\n");
				closesocket(iter->second);
				gameData->removePlayer(iter->first, ClientType::SERVER_SIDE);
				network->sessions.erase(iter++);
				sendActionPackets();
			}
			else
			{
				iter++;
			}

			continue;
		}
		moveForward = moveBackward = moveLeft = moveRight = false;

		bool disconnectedClient = false;
		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			if (packet.packet_type != INIT_CONNECTION && packet.id == -1) {
				continue;
			}

			switch (packet.packet_type) {
#ifdef USE_DEBUG_KEYS
			case OPEN_ALL_BOXES_EVENT:
				gameData->getAtlas()->openAllBoxes();
				break;
			case INCREMENT_ANGER_EVENT:
				if (gameData->getChefAnger() < CHEF_MAX_ANGER)
				{
					gameData->incrementChefAnger();
				}
				if (gameData->getChefVision() < CHEF_MAX_VISION)
				{
					gameData->incrementChefVision();
				}
				break;
			case UNLOCK_ALL_GATES_EVENT:
				gameData->getAtlas()->unlockAllGates();
				break;
#endif
			case INIT_CONNECTION:
				newPlayerInit = true;
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received init packet from client\n");
#endif
				initNewClient();
				sendInitPackets();
				break;

			case ACTION_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received action event packet from client\n");
#endif
				break;
#ifdef USE_DEBUG_KEYS
			case REMOVE_WALLS_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received remove walls event packet from client\n");
#endif
				{
					if (Player * player = gameData->getPlayer(playerID))
					{
						int row, col;
						Location loc = player->getLocation();
						gameData->getAtlas()->getMapCoords(loc, row, col);
						gameData->getAtlas()->removeWalls(row, col);
					}
				}
				break;
#endif
			case PLAYER_DASH_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received PlayerDash event packet from client\n");
#endif
				{
					if (Player * player = gameData->getPlayer(playerID))
					{
						
						if (player->getDashCooldown() <= 0 )
						{

							player->setDashing(true);
							player->setDashStartTime();
							player->setDashCooldownStartTime();
							player->setDashReady(false);


						}

					}
					
				}
				break;
			case GO_TO_CREDITS_EVENT:
				gameData->setGameState(GameState::END_CREDITS);
				break;
			case SET_DONE_LOADING_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SET_DONE_LOADING event packet from client\n");
#endif
				gameData->setGameState(GameState::IN_GAME);
				break;
			case DONE_LOADING_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received DONE_LOADING event packet from client\n");
#endif
				gameData->getPlayer(playerID)->setDoneLoading(true);
				gameData->checkAllPlayersLoaded();
				if(gameData->getAllPlayersLoaded())
					gameData->setGameState(GameState::IN_GAME);

				break;
			case START_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received START event packet from client\n");
#endif
				{
					for (auto iter = gameData->getAllPlayers().begin(); iter != gameData->getAllPlayers().end(); iter++)
					{
						Player * player = iter->second;
						if (player->hasSelectedAnimal())
						{
							player->setModelType(gameData->getAvailableCharacter());
						}
						else
						{
							player->setModelType(ModelType::CHEF);
							Location chefLocation(CHEF_SPAWN_COL * TILE_SIZE + TILE_SIZE/2, 0, CHEF_SPAWN_ROW * TILE_SIZE + TILE_SIZE / 2);
							Tile * tile = gameData->getTile(chefLocation);
							chefLocation.setY(tile->getHeight() / 2 * TILE_HEIGHT);
							player->setLocation(chefLocation);
						}
						updateHeight(iter->first);
					}
					gameData->setGameState(GameState::LOADING);

				}
				break;
			case SELECT_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT event packet from client\n");
#endif
				gameData->getPlayer(playerID)->toggleAnimalSelection();
				break;
#ifdef USE_DEBUG_KEYS
			case SELECT0_EVENT:
			{
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT0 event packet from client\n");
#endif
				//gameData->getPlayer(iter->first)->setModelType(ModelType::DEFAULT);
				auto iResult = shutdown(iter->second, SD_SEND);
				if (iResult == SOCKET_ERROR)
					closesocket(iter->second);
				gameData->removePlayer(iter->first, ClientType::SERVER_SIDE);
				network->sessions.erase(iter++);
				disconnectedClient = true;
			}
				break;
			case SELECT1_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT1 event packet from client\n");
#endif
				gameData->getPlayer(iter->first)->setModelType(ModelType::CHEF);
				break;
			case SELECT2_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT2 event packet from client\n");
#endif
				gameData->getPlayer(iter->first)->setModelType(ModelType::RACOON);
				break;
			case SELECT3_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT3 event packet from client\n");
#endif
				gameData->getPlayer(iter->first)->setModelType(ModelType::CAT);
				break;
			case SELECT4_EVENT:
#ifdef PRINT_DEBUG_STATEMENTS
				printf("server received SELECT4 event packet from client\n");
#endif
				gameData->getPlayer(iter->first)->setModelType(ModelType::DOG);
				break;
#endif
			case FORWARD_EVENT:
				if (gameStarted && !(chefWin || animalWin)) 
				{
					updateMovement(1, playerID);
				}
				break;
				
			case BACKWARD_EVENT:
				if (gameStarted && !(chefWin || animalWin))
				{
					updateMovement(2, playerID);
				}
				break;
				
			case LEFT_EVENT:
				if (gameStarted && !(chefWin || animalWin))
				{
					updateMovement(3, playerID);
				}
				break;
				
			case RIGHT_EVENT:
				if (gameStarted && !(chefWin || animalWin))
				{
					updateMovement(4, playerID);
				}
				break;
			case INTERACT_EVENT:
			{
				if (gameStarted && !(chefWin || animalWin))
				{

					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->isInteracting() ||
							player->isCaught() ||
							player->getHidden()) {
							break;
						}

						Location loc = player->getLocation();

						//easter egg
						if (gameData->getAtlas()->getTileAt(loc)->getTileType() == TileType::OBJECT) {
							std::cout << "bb: " << player->getBearBuff() << std::endl;
							if (dynamic_cast<ObjectTile *> (gameData->getAtlas()->getTileAt(loc))->getModel() == ItemModelType::painting &&
								(dynamic_cast<ObjectTile *> (gameData->getAtlas()->getTileAt(loc)))->getBearBuff() &&
								!player->getBearBuff()) {
								std::cout << "SETTING BEAR BUFF " << std::endl;
								player->setBearBuff(true);
								if (player->isChef()) {
									gameData->boostChefAnger();
								}
								else {
									player->resetSpeedMultiplier();
								}
							}
						}

						if (player->isChef())
						{
							Direction dir = player->getFacingDirection();
							Location jailLoc;
							JailTile * jailTile = gameData->getAdjacentJailTile(loc, dir, jailLoc);

							//drop off animal
							if (player->hasCaughtAnimal() && jailTile && jailTile->isJailEmpty())//gameData->getAtlas()->hasJail(loc) && (gameData->getAtlas()->isJailEmpty(loc)))
							{
								int animal = player->getCaughtAnimalId();
								//update jail with animal
								jailTile->placeAnimalInJail(animal);
								player->setCaughtAnimalId(-1);
								player->setCaughtAnimalType(ModelType::DEFAULT);

								player->setCaughtAnimal(false);

								//update animal's location to jail
								int x = (int)(jailLoc.getX() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
								int y = jailLoc.getY();
								int z = (int)(jailLoc.getZ() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
								gameData->getPlayer(animal)->setLocation(x, y, z);

							}
							else if (!player->hasCaughtAnimal())
							{
								if (!player->isInteracting())
								{
									player->setInteracting(true);
									player->setAction(Action::SWING_NET);
									player->setActionStartTime();
									bool animalCaught = false;


									chefWin = true;
									for (auto iter2 = gameData->getAllPlayers().begin(); iter2 != gameData->getAllPlayers().end(); iter2++)
									{
										if (iter2->first == iter->first)
										{
											continue;
										}

										Location tLoc = iter2->second->getLocation();

										//if (player->inRange(loc, tLoc) && !iter2->second->isCaught() && !player->hasCaughtAnimal())
										if (loc.distanceTo(tLoc) < CHEF_CATCH_RADIUS && gameData->getAtlas()->hasWallInBetween(loc, tLoc) && !iter2->second->isCaught() && !player->hasCaughtAnimal())
										{

											if (!(gameData->getAtlas()->tileHasItem(loc)))
											{
												ItemModelType itemName = iter2->second->getInventory();
												gameData->getAtlas()->updateTileItem(loc, itemName);
												iter2->second->setInventory(ItemModelType::EMPTY);
												iter2->second->resetSpeedMultiplier();

												gameData->getAtlas()->updateDroppedItem(itemName, loc);
											}
											else {
												ItemModelType playerItem = iter2->second->getInventory();
												ItemModelType tileItem = gameData->getAtlas()->getTileItem(loc);

												gameData->getAtlas()->returnItemToSpawn(tileItem);
												gameData->getAtlas()->updateTileItem(loc, playerItem);
												iter2->second->setInventory(ItemModelType::EMPTY);
												iter2->second->resetSpeedMultiplier();
												gameData->getAtlas()->updateDroppedItem(playerItem, loc);
											}

											if (gameData->getChefAnger() >= CHEF_MAX_ANGER && !animalCaught)
											{
												//get random jail
												//deploy animal in random jail
												auto jailLocations = gameData->getAtlas()->jailLocations;
												int randJailIndex = rand() % jailLocations.size();
												auto randJailCoords = jailLocations.at(randJailIndex);
												Location randJailLoc = Location(randJailCoords.second * TILE_SIZE, 0,  randJailCoords.first * TILE_SIZE);
												JailTile * randJailTile = gameData->getJailTile(randJailLoc);
												// Keep checking for a random empty jail
												while (!randJailTile->isJailEmpty())
												{
													randJailIndex = rand() % jailLocations.size();
													randJailCoords = jailLocations.at(randJailIndex);
													randJailLoc = Location(randJailCoords.second * TILE_SIZE, 0, randJailCoords.first * TILE_SIZE);
													randJailTile = gameData->getJailTile(randJailLoc);
												}

												if (randJailTile->isJailEmpty()) {
													iter2->second->setCaughtStatus(true);
													iter2->second->setLocation(randJailLoc.getX() + TILE_SIZE / 2, (float)(randJailTile->getHeight() / 2 * TILE_HEIGHT), randJailLoc.getZ() + TILE_SIZE / 2);
													iter2->second->setAction(Action::NONE);
													randJailTile->placeAnimalInJail(iter2->first);
													animalCaught = true;
												}
											}
											else
											{
												player->setCaughtAnimal(true);
												player->setCaughtAnimalId(iter2->first);
												player->setCaughtAnimalType(iter2->second->getModelType());
												iter2->second->setLocation(player->getLocation());
												iter2->second->setCaughtStatus(true);
												gameData->getPlayer(iter2->first)->setAction(Action::NONE);
											}
										}

										if (!iter2->second->isChef() && !iter2->second->isCaught())
										{
											chefWin = false;
										}

									}
								}
								if (chefWin)
								{
									gameData->setWT(WinType::CHEF_WIN);
								}
							}
						}
						else
						{
							Location loc = player->getLocation();
							ItemModelType item = gameData->getAtlas()->getTileItem(loc);
							Direction dir = player->getFacingDirection();
							Location objectLoc;
							Location jailLoc;
							ObjectTile * objectTile = gameData->getAdjacentObjectTile(loc, dir, objectLoc);

							if (player->getInventory() == ItemModelType::EMPTY && (item != ItemModelType::EMPTY || objectTile && objectTile->getItem() == ItemModelType::cake))
							{
								if (objectTile && objectTile->getItem() == ItemModelType::cake)
								{
									player->setInventory(objectTile->getItem());
									objectTile->setItem(ItemModelType::EMPTY);
									player->modifySpeedMultiplier(CAKE_SLOWDOWN_MULTIPLIER);


								}
								else if (item != ItemModelType::EMPTY)
								{
									player->setInventory(item);
									if(item == ItemModelType::cake)
										player->modifySpeedMultiplier(CAKE_SLOWDOWN_MULTIPLIER);
									else
										player->modifySpeedMultiplier(ITEM_SLOWDOWN_MULTIPLIER);
									gameData->getAtlas()->updateTileItem(loc, ItemModelType::EMPTY);
								}
							}
							else if (KeyDropTile * keyDropTile = gameData->getKeyDropTile(loc))
							{
								if (keyDropTile->isValidKey(player->getInventory()))
								{
									keyDropTile->updateKeyProgress(player->getInventory());
									gameData->updateGateProgress(keyDropTile->getGateNum());
									player->setAction(Action::KEY_DROP);
									player->setInventory(ItemModelType::EMPTY);
									player->resetSpeedMultiplier();
									player->setInteracting(true);
									player->setActionStartTime();
								}
							}
							else if (GateTile * gateTile = gameData->getGateTile(loc))
							{
								if (gateTile->hasAllKeys() && !gateTile->isOpen())
								{
									bool isFacingGate = false;
									std::bitset<4> wall(gateTile->getWall());
									Direction facingDirection = player->getFacingDirection();
									int gateRow = 0;
									int gateCol = 0;
									gameData->getAtlas()->getMapCoords(loc, gateRow, gateCol);
									Location gateLoc(0, gateTile->getHeight() / 2 * TILE_HEIGHT, 0);
									//check left wall
									if (wall[3]) {
										if (facingDirection == Direction::EAST ||
											facingDirection == Direction::NORTHEAST ||
											facingDirection == Direction::SOUTHEAST)
											isFacingGate = true;
										gateLoc.setX(gateCol * TILE_SIZE);
										gateLoc.setZ(gateRow * TILE_SIZE + TILE_SIZE/2);
									}
									//check up wall
									else if (wall[2]) {
										if (facingDirection == Direction::SOUTH ||
											facingDirection == Direction::SOUTHEAST ||
											facingDirection == Direction::SOUTHWEST)
											isFacingGate = true;
										gateLoc.setX(gateCol * TILE_SIZE + TILE_SIZE / 2);
										gateLoc.setZ(gateRow * TILE_SIZE);
									}
									//check down wall
									else if (wall[1]) {
										if (facingDirection == Direction::NORTH ||
											facingDirection == Direction::NORTHEAST ||
											facingDirection == Direction::NORTHWEST)
											isFacingGate = true;
										gateLoc.setX(gateCol * TILE_SIZE + TILE_SIZE / 2);
										gateLoc.setZ(gateRow * TILE_SIZE + TILE_SIZE);
									}
									//check right wall
									else if (wall[0]) {
										if (facingDirection == Direction::WEST ||
											facingDirection == Direction::NORTHWEST||
											facingDirection == Direction::SOUTHWEST)
											isFacingGate = true;
										gateLoc.setX(gateCol * TILE_SIZE + TILE_SIZE);
										gateLoc.setZ(gateRow * TILE_SIZE + TILE_SIZE / 2);
									}
									if (isFacingGate && loc.distanceTo(gateLoc) < DISTANCE_TO_CONSTRUCT_GATE)
									{
										player->setAction(Action::CONSTRUCT_GATE);
										player->setInteracting(true);
										player->setActionStartTime();
									}
								}
								if (gateTile->hasAllKeys() && gateTile->isOpen())
								{
									if (player->getInventory() == ItemModelType::cake)
									{
										int gateNum = gameData->getGateTile(loc)->getGateNum();
										if (gateNum == 1) { //door
											gameData->setWT(WinType::DOOR);
										}
										else if (gateNum == 2) { //bathroom
											gameData->setWT(WinType::TOILET);
										}
										else if (gateNum == 3) { //vent
											gameData->setWT(WinType::VENT);
										}

										animalWin = true;
									}

								}
							}
							else if (JailTile * jailTile = gameData->getAdjacentJailTile(loc, dir, jailLoc))
							{
								double seconds = player->getInteractingTime(1);

								// Only allow another jail action if enough time has passed
								if (seconds > UNLOCK_JAIL_COOLDOWN && !jailTile->isJailEmpty()) {
			

									player->setAction(Action::UNLOCK_JAIL);
									player->setInteracting(true);
									player->setActionStartTime();
									player->setUnlockJailStartTime();

									//update jail progres

									jailTile->unlockJail();

									//check if jail progress == 5
									if (jailTile->getProgress() >= HITS_TO_OPEN_JAIL)
									{
										//update animal 
										int animal = jailTile->getCapturedAnimal();
										gameData->getPlayer(animal)->setCaughtStatus(false);

										//output animal to adjacent tile
										Location tileLoc;
										Tile * tile = gameData->getAdjacentTileNotThroughWalls(jailLoc, tileLoc);
										gameData->getPlayer(animal)->setLocation(tileLoc.getX(), tileLoc.getY(), tileLoc.getZ());
									
										//update jail
										jailTile->resetJail();

									}
								}
							}
							else if (gameData->getAtlas()->hasBox(loc))
							{
								if (!player->isInteracting()) {

									if (player->getInstantSearch())
									{
										gameData->getAtlas()->updateBoxLayout(loc);
									}
									else
									{
										player->setInteracting(true);
										player->setAction(Action::OPEN_BOX);
										player->setActionStartTime();
									}
								}
							}
						}
					}
				}
				break;
			}
			case POWERUP_EVENT:
			{
				Player * player = gameData->getPlayer(iter->first);
				if (player->isInteracting() ||
					player->isCaught() ||
					player->getHidden()) {
					break;
				}

				if (!player->isChef()) {
					if (!player->isInteracting() && !player->getPoweringUp()) {
						player->setInteracting(true);
						player->setPoweringUp(true);
						player->setActionStartTime();
					}
				}
				break;
			}
			case RELEASE_EVENT:
			{
				if (gameStarted && !(chefWin || animalWin))
				{

					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->isCaught() ||
							player->getHidden()) {
							break;
						}

						Location loc = player->getLocation();

// HEAD
						if (player->isInteracting()) {
							double seconds = player->getInteractingTime(0);

							if (player->getAction() == Action::OPEN_BOX)
							{
								if (seconds > TIME_TO_OPEN_BOX) {
									gameData->getAtlas()->updateBoxLayout(loc);
									player->setInteracting(false);
									player->setAction(Action::NONE);
								}
							}
							else if (player->getAction() == Action::CONSTRUCT_GATE)
							{
								if (!player->isChef() && gameData->getAtlas()->hasGate(loc))
								{
									GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
									if (!gateTile->isOpen())
									{
										player->setInteracting(false);
										player->setAction(Action::NONE);

										//update progress of gate 
										double seconds = player->getInteractingTime(0);
										gateTile->constructGate(seconds);
									}
								}
							}

							if (!player->isChef()) 
							{
								player->setInteracting(false);
								player->setAction(Action::NONE);
							}
							player->setPoweringUp(false);
						}
					}
				}
			
				break;
			}
			case DROP_EVENT:
			{
				if (gameStarted && !(chefWin || animalWin))
				{

					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->isInteracting() ||
							player->isCaught() ||
							player->getHidden()) {
							break;
						}

						Location loc = player->getLocation();

						// PLayer cannot drop item if there is an item already on the current tile
						if (!(gameData->getAtlas()->tileHasItem(loc)))
						{
							ItemModelType itemName = player->getInventory();
							gameData->getAtlas()->updateTileItem(loc, itemName);
							player->setInventory(ItemModelType::EMPTY);
							player->resetSpeedMultiplier();

							gameData->getAtlas()->updateDroppedItem(itemName, loc);
						}
					}
				}
				break;
			}
			case HIDE_EVENT:
			{
				if (chefWin || animalWin) { break; }

				if (Player * player = gameData->getPlayer(iter->first))
				{
					if (player->isInteracting() ||
						player->isCaught()) {
						break;
					}

					Location loc = player->getLocation();
					if (!(gameData->getAtlas()->hasHide(loc))) { return; }

					HideTile * hideTile = (HideTile *)(gameData->getAtlas()->getTileAt(loc));

					if (player->isChef()) {
						if (!hideTile->checkHideTileEmpty())
						{
							int animal = hideTile->getAnimalHiding();
							gameData->getPlayer(animal)->setCaughtStatus(true);
							player->setCaughtAnimalId(animal);
							player->setCaughtAnimal(true);
							player->setHidden(false);
							hideTile->setAnimalHiding(-1);
						}
					}
					else {
						if (player->getHidden())
						{
							player->setHidden(false);
							hideTile->setAnimalHiding(-1);
						}
						else if (hideTile->checkHideTileEmpty())
						{
							//set hideTile full
							hideTile->setAnimalHiding(iter->first);
							//set animal to hidden
							player->setHidden(true);
						}
					}
				}
				break;
			}
			case RESET_EVENT:
			{
				if (chefWin || animalWin) { 
					chefWin = false;
					animalWin = false;
					gameData->setWT(WinType::NONE);
					resetGame();
					gameData->setGameState(GameState::IN_LOBBY);
				}
				

				break;
			}

			default:
				break;
			}

			if (disconnectedClient)
			{
				break;
			}
		}
		if (disconnectedClient)
			continue;
		if (Player * player = gameData->getPlayer(iter->first))
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
				updateMovement2(facingDirection, playerID);
			}
			
		}
		sendActionPackets(); // sends data after processing input from one clientss
		iter++;
	}

	if (gameData->getGameState() == GameState::IN_GAME)
	{

		if (chefWin || animalWin)
		{
			gameData->setGameState(GameState::WIN_CUTSCENE);
		}
		gameData->getAtlas()->checkDroppedItems();
		if (gameData->getAtlas()->checkBoxRespawn())
		{
			sendActionPackets();
		}
		if (gameData->countdownStarted() && !gameData->countdownDone())
		{
			sendActionPackets();
		}

		if (gameData->countdownDone())
		{
			gameStarted = true;
		}

		//All server loop checks 

		if (gameData->getGameClock() % CHEF_ANGER_INTERVAL == 0) {
			if (gameData->getGameClock() > gameData->getCurrentTime())
			{
				bool sendUpdate = false;
				if (gameData->getChefAnger() < CHEF_MAX_ANGER)
				{
					gameData->incrementChefAnger();
					sendUpdate = true;
				}
				if (gameData->getChefVision() < CHEF_MAX_VISION)
				{
					gameData->incrementChefVision();
					sendUpdate = true;
				}

				if (sendUpdate)
				{
					sendActionPackets();
				}
				gameData->setCurrentTime();
			}
		}

		for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
		{
			if (Player * player = gameData->getPlayer(iter->first))
			{
				int prevDashCooldown = player->getDashCooldown();
				player->updateDashCooldownTime();
				// Only send dash cooldown updates when it is visibly updated int-wise
				if (!player->isDashReady() && player->getDashCooldown() >= 0 && prevDashCooldown != player->getDashCooldown())
				{
					sendActionPackets();
					// Set dash to ready so that updates stop getting sent
					if (player->getDashCooldown() <= 0)
						player->setDashReady(true);
				}
				Location loc = player->getLocation();

				if (player->isChef()) {
					player->updateChefMultiplier(gameData->getChefAnger());
				}

				if (player->isCaught())
				{
					continue;
				}

				if (player->getReverseChef() && player->getReverseTime() > MAX_REVERSE_CHEF_TIME)
				{
					player->setReverseChef(false);
					gameData->setReverseChef(false);
					player->setPowerUp(PowerUp::NONE);
				}

				if (player->getSlowChef() && player->getSlowTime() > MAX_SLOW_CHEF_TIME)
				{
					player->setSlowChef(false);
					gameData->setSlowChef(false);
					player->setPowerUp(PowerUp::NONE);
				}

				if (player->getGhost() && player->getSpeedTime() > MAX_ANIMAL_GHOST_TIME)
				{

					player->setGhost(false);
					player->setPowerUp(PowerUp::NONE);
				}

				if (player->getInstantSearch() && player->getSearchTime() > MAX_ANIMAL_SEARCH_TIME)
				{
					player->setInstantSearch(false);
					player->setPowerUp(PowerUp::NONE);
				}


				if (player->isDashing() && player->getDashTime() > MAX_PLAYER_DASH_TIME)
				{

					player->setDashing(false);
				}
				if (player->isInteracting())
				{
					double seconds = player->getInteractingTime(0);
					if (player->isChef())
					{

						if (player->getAction() == Action::SWING_NET)
						{
							if (seconds > SWING_NET_DELAY) {
								player->setInteracting(false);
								player->setAction(Action::NONE);
								sendActionPackets();
							}
						}
						
						else if (player->getAction() == Action::DESTROY_POWERUP)
						{
							double destroyPowerUpSeconds = player->getInteractingTime(2);
							if (destroyPowerUpSeconds > DESTROY_POWERUP_DELAY)
							{
								player->setAction(Action::NONE);
								player->setInteracting(false);
								sendActionPackets();
							}
							
						}
					}
					else if (player->getAction() == Action::OPEN_BOX)
					{
						// HEAD
						if (seconds > TIME_TO_OPEN_BOX) {
							gameData->getAtlas()->updateBoxLayout(loc);
							player->setInteracting(false);
							player->setAction(Action::NONE);
							sendActionPackets();
						}
					}
					else if (player->getAction() == Action::UNLOCK_JAIL)
					{
						double seconds = player->getInteractingTime(0);
						if (!player->isChef())
						{
							if (seconds > UNLOCK_JAIL_DELAY) {
								player->setInteracting(false);
								player->setAction(Action::NONE);
								sendActionPackets();
							}
						}
					}
					else if (player->getAction() == Action::KEY_DROP)
					{
						double seconds = player->getInteractingTime(0);
						if (!player->isChef())
						{
							if (seconds > KEY_DROP_DELAY) {
								player->setInteracting(false);
								player->setAction(Action::NONE);
								sendActionPackets();
							}
						}
					}
					else if (player->getAction() == Action::CONSTRUCT_GATE)
					{
						if (gameData->getAtlas()->hasGate(loc))
						{
							GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));

							if (!gateTile->isOpen())
							{
								double seconds = player->getInteractingTime(0);

								if (gateTile->getCurrentConstructTime() + seconds >= TIME_TO_CONSTRUCT_GATE)
								{
									gateTile->constructGate(seconds);
									player->setInteracting(false);
									player->setAction(Action::NONE);
									sendActionPackets();
								}
							}
						}
					}

					//Handling power ups
					if (player->getPoweringUp() && seconds > ABILITY_CHARGE_TIME) {
						player->setInteracting(false);
						player->setPoweringUp(false);


						ItemModelType it = player->getInventory();
						bool powerUp = true;
						if (it == ItemModelType::apple)
						{
							//call limit chef vision
							player->setReverseChef(true);
							gameData->setReverseChef(true);
							player->setReverseStartTime();
							player->setPowerUp(PowerUp::CHEF_REVERSE);
						}
						else if (it == ItemModelType::orange)
						{
							//call limit chef speed 
							player->setSlowChef(true);
							gameData->setSlowChef(true);
							player->setSlowStartTime();
							player->setPowerUp(PowerUp::CHEF_SLOW);
						}
						else if (it == ItemModelType::bananaGreen)
						{
							//call ghost because green means go 
							player->setGhost(true);
							player->setSpeedStartTime();
							player->setPowerUp(PowerUp::GHOST);
						}
						else if (it == ItemModelType::bananaPerfect)
						{
							//call flash because yellow banana
							float x = player->getLocation().getX();
							float y = player->getLocation().getY();
							float z = player->getLocation().getZ();
							if (player->getFacingDirection() == Direction::NORTH)
							{
								z += FLASH_DISTANCE;
							}
							else if (player->getFacingDirection() == Direction::SOUTH)
							{
								z -= FLASH_DISTANCE;
							}
							else if (player->getFacingDirection() == Direction::EAST)
							{
								x -= FLASH_DISTANCE;
							}
							else if (player->getFacingDirection() == Direction::WEST)
							{
								x += FLASH_DISTANCE;
							}
							else if (player->getFacingDirection() == Direction::NORTHEAST)
							{
								z += FLASH_DISTANCE / 1.5;
								x -= FLASH_DISTANCE / 1.5;
							}
							else if (player->getFacingDirection() == Direction::NORTHWEST)
							{
								z += FLASH_DISTANCE / 1.5;
								x += FLASH_DISTANCE / 1.5;
							}
							else if (player->getFacingDirection() == Direction::SOUTHEAST)
							{
								z -= FLASH_DISTANCE / 1.5;
								x -= FLASH_DISTANCE / 1.5;
							}
							else if (player->getFacingDirection() == Direction::SOUTHWEST)
							{
								z -= FLASH_DISTANCE / 1.5;
								x += FLASH_DISTANCE / 1.5;
							}
							if (x > TILE_SIZE * gameData->getAtlas()->getServerTileLayout()[0].size())
							{
								x = TILE_SIZE * gameData->getAtlas()->getServerTileLayout()[0].size() - TILE_SIZE / 2;
							}
							if (z > TILE_SIZE * gameData->getAtlas()->getServerTileLayout().size())
							{
								z = TILE_SIZE * gameData->getAtlas()->getServerTileLayout().size() - TILE_SIZE / 2;
							}
							if (x < 0) x = TILE_SIZE / 2;
							if (z < 0) z = TILE_SIZE / 2;

							Location tileLoc = Location();
							tileLoc.update(x, y, z);
							Tile * tile = gameData->getAtlas()->getTileAt(tileLoc);
							player->setLocation(x, tile->getHeight() / 2 * TILE_HEIGHT, z);
							player->setPowerUp(PowerUp::FLASH);
							sendActionPackets();
							player->setPowerUp(PowerUp::NONE);
						}
						else if (it == ItemModelType::bananaVeryRipe)
						{
							player->setInstantSearch(true);
							//maybe like a trap item - makes chef slip or creates a barrier for a set duration
							player->setSearchStartTime();
							player->setPowerUp(PowerUp::INSTA_SEARCH);
						}
						else {
							powerUp = false;
						}
						if (powerUp) {
							player->setInventory(ItemModelType::EMPTY);
							player->resetSpeedMultiplier();
							sendActionPackets();
						}
					}
				}

				double destroyPowerUpSeconds = player->getInteractingTime(2);
				if (destroyPowerUpSeconds > DESTROY_POWERUP_DELAY && player->getAction() == Action::DESTROY_POWERUP)
				{
					player->setAction(Action::NONE);
					sendActionPackets();
				}
			}
		}
	}
}

void ServerGame::sendInitPackets()
{
	std::string msg_string = "init: " + std::to_string(client_id) + "\n";
	client_id++;

	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	for (int i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	network->sendToAll(msg, packet_size);
	delete[] msg;
}

void ServerGame::sendActionPackets()
{
	// gets an encoded game data string for all players and general game data
	std::string msg_string = gameData->encodeGameData(newPlayerInit);
	newPlayerInit = false;
	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	for (int i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	network->sendToAll(msg, packet_size);
	delete[] msg;
}

void ServerGame::initNewClient()
{
	//updating current data structure to hold onto client
	gameData->addNewPlayer(client_id, Location(30, 0, 30), ClientType::SERVER_SIDE);
}

void ServerGame::updateMovement2(Direction dir, int id)
{
	Player * player = gameData->getPlayer(id);
	if (gameData->getPlayer(id)->isInteracting() ||
		gameData->getPlayer(id)->isCaught() ||
		gameData->getPlayer(id)->getHidden()) {
		return;
	}

	Location loc = gameData->getPlayer(id)->getLocation();
	double dashMultiplier = 1.0f;
	float xSPEED = 0.0f;
	float zSPEED = 0.0f;
	switch (dir)
	{
	case Direction::NORTH:
		zSPEED = sSpeed;
		break;
	case Direction::SOUTH:
		zSPEED = -sSpeed;
		break;
	case Direction::EAST:
		xSPEED = -sSpeed;
		break;
	case Direction::WEST:
		xSPEED = sSpeed;
		break;
	case Direction::NORTHEAST:
		zSPEED = dSpeed;
		xSPEED = -dSpeed;
		break;
	case Direction::NORTHWEST:
		zSPEED = dSpeed;
		xSPEED = dSpeed;
		break;
	case Direction::SOUTHEAST:
		zSPEED = -dSpeed;
		xSPEED = -dSpeed;
		break;
	case Direction::SOUTHWEST:
		zSPEED = -dSpeed;
		xSPEED = dSpeed;
		break;
	}

	Location destLoc;
	if (gameData->getPlayer(id)->isChef())
	{
		double multiplier = gameData->getPlayer(id)->getChefSpeedMultiplier();
		/*gameData->getPlayer(id)->setLocation(loc.getX() + (xSPEED*multiplier), loc.getY(), 
												loc.getZ() + (zSPEED*multiplier));*/
		if (gameData->getSlowChef()) {
			multiplier *= LIMIT_CHEF_SLOW_MULT;
		}
		destLoc = Location(loc.getX() + (xSPEED*multiplier), loc.getY(),
			loc.getZ() + (zSPEED*multiplier));

		
		
		Location loc = gameData->getPlayer(id)->getLocation();
		Tile * tile = gameData->getTile(loc);
		int row = 0;
		int col = 0;

		gameData->getAtlas()->getMapCoords(loc, row, col);
		Location tileCenter = Location(col * TILE_SIZE + TILE_SIZE / 2, 0, row  * TILE_SIZE + TILE_SIZE / 2);
		
		
		if (tile->getItem() != ItemModelType::EMPTY && gameData->getAtlas()->isItemPowerUp(tile->getItem()))
		{

			// Get distance from tile center to player, ignoring y value
			double dist = tileCenter.distanceTo(Location(loc.getX(), 0, loc.getZ()));

			// Only remove powerup item if the chef is near the center of tile where the item is
			if (dist < PLAYER_RADIUS*2) {
				tile->setItem(ItemModelType::EMPTY);
				gameData->getPlayer(id)->setAction(Action::DESTROY_POWERUP);
				player->setDestroyPowerUpStartTime();
				player->setInteracting(true);
			}
		}
	}
	else
	{
		if (player->isDashing())
			dashMultiplier = PLAYER_DASH_MULTIPLIER;
		if (gameData->getPlayer(id)->getGhost())
		{
			//gameData->getPlayer(id)->setLocation(loc.getX() + xSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER * dashMultiplier, loc.getY(),
			//										loc.getZ() + zSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER * dashMultiplier);
			destLoc = Location(loc.getX() + xSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER * dashMultiplier, loc.getY(),
				loc.getZ() + zSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER * dashMultiplier);
		}
		else
		{
			/*gameData->getPlayer(id)->setLocation(loc.getX() + xSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * dashMultiplier, loc.getY(),
													loc.getZ() + zSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * dashMultiplier);
			*/
			destLoc = Location(loc.getX() + xSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * dashMultiplier, loc.getY(),
													loc.getZ() + zSPEED * gameData->getPlayer(id)->getSpeedMultiplier() * dashMultiplier);
		}
	}

	if (gameData->getAtlas()->canMoveToDestination(loc, destLoc))
		gameData->getPlayer(id)->setLocation(destLoc);
	updatePlayerCollision(id, dir);
	//updateCollision(id);
	updateHeight(id);
}
void ServerGame::updateMovement(int dir, int id)
{
	if (gameData->getPlayer(id)->isCaught() ||
		gameData->getPlayer(id)->getHidden()) {
		return;
	}

	if (gameData->getReverseChef() && gameData->getPlayer(id)->isChef()) {
		if (dir % 2 == 1) 
			dir++;
		else 
			dir--;
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

}

void ServerGame::updateRightEvent(int id)
{
	moveRight = true;

}

void ServerGame::updateBackwardEvent(int id)
{
	moveBackward = true;
}

void ServerGame::updateForwardEvent(int id)
{
	moveForward = true;
}

void ServerGame::updateLeftEvent(int id)
{
	moveLeft = true;

}

void ServerGame::updateHeight(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	if (!gameData->getAtlas()->hasRamp(loc)) 
	{ 
		int y = gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 * TILE_HEIGHT;
		gameData->getPlayer(id)->setLocation(loc.getX(), y, loc.getZ());

		// Update location of captured animal to the chef's location
		if (gameData->getPlayer(id)->isChef())
		{
			if (gameData->getPlayer(id)->hasCaughtAnimal())
			{
				int animalId = gameData->getPlayer(id)->getCaughtAnimalId();
				gameData->getPlayer(animalId)->setLocation(gameData->getPlayer(id)->getLocation());
			}
		}

		if (!gameData->getPlayer(id)->isChef())
		{
			gameData->getPlayer(id)->setVisionRadius(y * 2 + DEFAULT_VISION);
		}
		else
		{
			gameData->setChefRampVision(y * 2);
		}

		return; 
	}

	float x = loc.getX();
	float y;
	float z = loc.getZ();

	RampTile * rampTile = (RampTile *)(gameData->getAtlas()->getTileAt(loc));

	if (rampTile->getRampDirection() == Orientation::NORTH)
	{
		/*y = (int) (TILE_SIZE - z % TILE_SIZE) * ((double) TILE_HEIGHT / TILE_SIZE) 
			* (gameData->getAtlas()->getTileAt(loc)->getHeight()/2 +1);*/
		
		y = (TILE_SIZE - (z - (int)(z / TILE_SIZE) * TILE_SIZE)) * ((double) TILE_HEIGHT / TILE_SIZE) 
			* (gameData->getAtlas()->getTileAt(loc)->getHeight()/2 +1);
	}
	else if (rampTile->getRampDirection() == Orientation::SOUTH)
	{
		y = (z - (int)(z / TILE_SIZE) * TILE_SIZE) * ((double) TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	else if (rampTile->getRampDirection() == Orientation::EAST)
	{
		y = ((x - (int)(x / TILE_SIZE) * TILE_SIZE)) * ((double)TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	else if (rampTile->getRampDirection() == Orientation::WEST)
	{
		y = (TILE_SIZE - (x - (int)(x / TILE_SIZE) * TILE_SIZE)) * ((double)TILE_HEIGHT / TILE_SIZE)
			* (gameData->getAtlas()->getTileAt(loc)->getHeight() / 2 + 1);
	}
	gameData->getPlayer(id)->setLocation(x, y, z);

	if (!gameData->getPlayer(id)->isChef()) 
	{
		gameData->getPlayer(id)->setVisionRadius(y*2  + DEFAULT_VISION);
	}
	else
	{
		gameData->setChefRampVision(y*2);
	}

	// Update location of captured animal to the chef's location
	if (gameData->getPlayer(id)->isChef())
	{
		if (gameData->getPlayer(id)->hasCaughtAnimal())
		{
			int animalId = gameData->getPlayer(id)->getCaughtAnimalId();
			gameData->getPlayer(animalId)->setLocation(gameData->getPlayer(id)->getLocation());
		}
	}
}

void ServerGame::updatePlayerCollision(int id, Direction dir) 
{
	Location pLoc = gameData->getPlayer(id)->getLocation();
	std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	
	//0 == right
	//1 == down
	//2 == forward
	//3 == left
	//dmap<string, vector<int>>::iterator it;
	Player * player = gameData->getPlayer(id);
	std::map < int, Player * > players = gameData->getAllPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (it->first == id) {
			continue;
		}

		// Ignore player collision check for the animal that the chef is carrying
		if (player->isChef() && player->hasCaughtAnimal() && player->getCaughtAnimalId() == it->first) {
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
			/*if (dir == 0) 
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
			else if(dir == 3)
			{
				loc[0] = ot_x + 2 * PLAYER_RADIUS;
			}*/

			switch (dir)
			{
			case Direction::NORTH:
				loc[2] = ot_z - 2 * PLAYER_RADIUS;
				break;
			case Direction::SOUTH:
				loc[2] = ot_z + 2 * PLAYER_RADIUS;
				break;
			case Direction::EAST:
				loc[0] = ot_x + 2 * PLAYER_RADIUS;
				break;
			case Direction::WEST:
				loc[0] = ot_x - 2 * PLAYER_RADIUS;
				break;
			case Direction::NORTHEAST:
				loc[2] = ot_z - 2 * PLAYER_RADIUS;
				my_x = loc[0];
				my_z = loc[2];
				dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
				if (dist < 2 * PLAYER_RADIUS)
					loc[0] = ot_x + 2 * PLAYER_RADIUS;
				break;
			case Direction::NORTHWEST:
				loc[2] = ot_z - 2 * PLAYER_RADIUS;
				my_x = loc[0];
				my_z = loc[2];
				dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
				if (dist < 2 * PLAYER_RADIUS)
					loc[0] = ot_x - 2 * PLAYER_RADIUS;
				break;
			case Direction::SOUTHEAST:
				loc[2] = ot_z + 2 * PLAYER_RADIUS;
				my_x = loc[0];
				my_z = loc[2];
				dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
				if (dist < 2 * PLAYER_RADIUS)
					loc[0] = ot_x + 2 * PLAYER_RADIUS;
				break;
			case Direction::SOUTHWEST:
				loc[2] = ot_z + 2 * PLAYER_RADIUS;
				my_x = loc[0];
				my_z = loc[2];
				dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
				if (dist < 2 * PLAYER_RADIUS)
					loc[0] = ot_x - 2 * PLAYER_RADIUS;
				break;
			}
			gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
		}
	}
}

void ServerGame::updateCollision(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getAtlas()->detectWallCollision(loc);
	gameData->getPlayer(id)->setLocation(loc);
	
	gameData->getAtlas()->detectObjectCollision(loc);
	gameData->getPlayer(id)->setLocation(loc);


}

void ServerGame::resetGame() 
{
	delete gameData;
	gameData = new GameData(SERVER_GAMEDATA);

	//reset players
	int count = 0;
	std::map<unsigned int, SOCKET>::iterator iter;
	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		unsigned int id = iter->first;
		Location loc = gameData->initLocs[count];
		gameData->addNewPlayer(id, loc, ClientType::SERVER_SIDE);
		count++;
	}
}
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
    client_id = 0;
 
	bool chefWin = false;
	bool animalWin = false;
    // set up the server network to listen 
    network = new ServerNetwork(); 
	//atlas = new Atlas();
	gameData = new GameData(SERVER_GAMEDATA);
}
 
void ServerGame::update() 
{
	auto start_time = std::chrono::high_resolution_clock::now();

    // get new clients
    if(network->acceptNewClient(client_id))
    {
        printf("client %d has been connected to the server\n",client_id); 
    }
	receiveFromClients();
	//sendActionPackets();

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	//std::cout << " took " << time / std::chrono::milliseconds(1) << "ms to run.\n";
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
		//cout << "size:" << network->sessions.size() << std::endl;
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
			case INIT_CONNECTION:
				newPlayerInit = true;
				printf("server received init packet from client\n");
				initNewClient();
				sendInitPackets();
				break;

			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				break;
			case READY_EVENT:
				printf("server received ENTER event packet from client\n");
				if(!gameStarted)
					gameData->getPlayer(iter->first)->toggleReady();
				break;
			case START_EVENT:
				printf("server received START event packet from client\n");
				if (!gameStarted && allPlayersReady)
				{
					initCharacters = true;
					gameData->startCountdown();
				}
				break;
			case SELECT_EVENT:
				printf("server received SELECT event packet from client\n");
				break;
			case SELECT0_EVENT:
			{
				printf("server received SELECT0 event packet from client\n");
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
				printf("server received SELECT1 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::CHEF);
				break;
			case SELECT2_EVENT:
				printf("server received SELECT2 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::RACOON);
				break;
			case SELECT3_EVENT:
				printf("server received SELECT3 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::CAT);
				break;
			case SELECT4_EVENT:
				printf("server received SELECT4 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::DOG);
				break;
				/*if (gameStarted && !(chefWin || animalWin))
				{
					if (chefWin || animalWin) { break; }*/
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
						/*if (player->isChef())
						{
							*///if (player->getIsCaught()) { break; }

							//Location loc = player->getLocation();
							if (player->isChef())
							{
								std::cout << "CAUGHT ANIMAL" << std::endl;
								Direction dir = player->getFacingDirection();
								Location jailLoc;
								JailTile * jailTile = gameData->getAdjacentJailTile(loc, dir, jailLoc);

								//drop off animal
								if (player->hasCaughtAnimal() && jailTile && jailTile->isJailEmpty())//gameData->getAtlas()->hasJail(loc) && (gameData->getAtlas()->isJailEmpty(loc)))
								{
									int animal = player->getCaughtAnimalId();
									//update jail with animal
									std::cout << "PLACE ANIMAL IN JAIL" << std::endl;
									//gameData->getAtlas()->placeInJail(loc, animal);
									jailTile->placeAnimalInJail(animal);
									player->setCaughtAnimal(false);

									//update animal's location to jail
									int x = (int)(jailLoc.getX() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
									int y = jailLoc.getY();
									int z = (int)(jailLoc.getZ() / TILE_SIZE) * TILE_SIZE + (int)(TILE_SIZE / 2);
									gameData->getPlayer(animal)->setLocation(x, y, z);

								}
								else
								{
									std::cout << "SWINGING" << std::endl;
									if (!player->isInteracting())
									{
										std::cout << "CAN SWING" << std::endl;
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
// HEAD
											/*if (player->inRange(loc, tLoc) && !iter2->second->isCaught())
											{
												player->setCaughtAnimal(true);
												player->setCaughtAnimalId(iter2->first);
												iter2->second->setCaughtStatus(true);*/
//=
											if (player->inRange(loc, tLoc) && !iter2->second->isCaught())
											{
												if (gameData->getChefAnger() >= gameData->getMaxAnger() - 5 && !animalCaught) 
												{
													//get random jail
													//deploy animal in random jail
													auto jailLocations = gameData->getAtlas()->jailLocations;
													for (auto it = jailLocations.begin();
														it != jailLocations.end();  // Use (), and assuming itt was a typo
														it++)
													{
														Location jLoc = Location(it->second, 0, it->first);
														if (gameData->getAtlas()->hasJail(jLoc)) {
															JailTile * jailTile = gameData->getJailTile(jLoc);
															if (jailTile->isJailEmpty()) {
																iter2->second->setCaughtStatus(true);
																iter2->second->setLocation(jLoc.getX()+TILE_SIZE/2, (float)jailTile->getHeight(), jLoc.getZ()+TILE_SIZE/2);
																jailTile->placeAnimalInJail(iter2->first);
																animalCaught = true;
																break;
															}
														}
													}
												}
												else
												{
													player->setCaughtAnimal(true);
													player->setCaughtAnimalId(iter2->first);
													iter2->second->setLocation(player->getLocation());
													iter2->second->setCaughtStatus(true);
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
										std::cout << "CHEF WIN" << std::endl;
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
									// Pr
									if (objectTile && objectTile->getItem() == ItemModelType::cake)
									{
										std::cout << "table has cake!" << std::endl;
										player->setInventory(objectTile->getItem());
										objectTile->setItem(ItemModelType::EMPTY);
										player->setSpeedMultiplier(CAKE_SLOWDOWN_MULTIPLIER);


									}
									else if (item != ItemModelType::EMPTY)
									{
										player->setInventory(item);
										player->setSpeedMultiplier(ITEM_SLOWDOWN_MULTIPLIER);
										gameData->getAtlas()->updateTileItem(loc, ItemModelType::EMPTY);
									}
								}
								//else if (gameData->getAtlas()->hasGate(loc))
								else if (KeyDropTile * keyDropTile = gameData->getKeyDropTile(loc))
								{
									if (keyDropTile->isValidKey(static_cast<Key>(player->getInventory())))
									{

										keyDropTile->updateKeyProgress(static_cast<Key>(player->getInventory()));
										gameData->updateGateProgress(keyDropTile->getGateNum());
										player->setInventory(ItemModelType::EMPTY);
										player->setSpeedMultiplier(1.0);

									}
								}
								else if (GateTile * gateTile = gameData->getGateTile(loc))
								{
									
									//GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
									/*if (gateTile->isValidKey(static_cast<Key>(player->getInventory())))
									{

										gateTile->updateKeyProgress(static_cast<Key>(player->getInventory()));
										player->setInventory(ItemModelType::EMPTY);
									}*/
									if (gateTile->hasAllKeys() /*&& !player->getOpeningGate()*/ && !gateTile->isOpen())
									{

										//player->setOpeningGate(true);
										player->setAction(Action::CONSTRUCT_GATE);
										player->setInteracting(true);
										player->setActionStartTime();
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
											std::cout << "ANIMAL WIN" << std::endl;
										}

									}
								}
								//else if (gameData->getAtlas()->hasJail(loc))
								//else if (JailTile * jailTile = gameData->getJailTile(loc))
								else if (JailTile * jailTile = gameData->getAdjacentJailTile(loc, dir, jailLoc))
								{
									//JailTile * jailTile = (JailTile *)(gameData->getAtlas()->getTileAt(loc));
									//player->setOpenJail(true);
									double seconds = player->getInteractingTime(1);
									
									// Only allow another jail action if enough time has passed
									if (seconds > UNLOCK_JAIL_COOLDOWN) {

										player->setAction(Action::UNLOCK_JAIL);
										player->setInteracting(true);
										player->setActionStartTime();
										player->setUnlockJailStartTime();

										//update jail progres
										std::cout << "UNLOCKING JAIL" << std::endl;

										jailTile->unlockJail();

										//check if jail progress == 5
										if (jailTile->getProgress() >= 5)
										{
											std::cout << "ANIMAL IS RELEASED" << std::endl;
											//update animal 
											int animal = jailTile->getCapturedAnimal();
											gameData->getPlayer(animal)->setCaughtStatus(false);

											//update jail
											jailTile->resetJail();

										}
									}
								}
								else if (gameData->getAtlas()->hasBox(loc))
								{
									std::cout << "HAS BOX" << std::endl;
									if (!player->isInteracting()) {
										std::cout << "starting to interact!" << std::endl;
// HEAD
										/*player->setInteracting(true);
										player->setAction(Action::OPEN_BOX);
										player->setActionStartTime();*/
//

										if (player->getInstantSearch()) 
										{
											gameData->getAtlas()->updateBoxLayout(loc);
										}
										else
										{
											player->setInteracting(true);
											//player->setOpeningBox(true);
											player->setAction(Action::OPEN_BOX);
											player->setActionStartTime();
										}
// server
									}
								}
							}
						//}
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
						std::cout << "starting to use powerup!" << std::endl;
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
									std::cout << "UPDATED BOX" << std::endl;
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
										std::cout << "RELEASED SPACE" << std::endl;
										player->setInteracting(false);
										player->setAction(Action::NONE);

										//update progress of gate 
										double seconds = player->getInteractingTime(0);
										gateTile->constructGate(seconds);
									}
								}
							}

							if (!player->isChef()) {
								std::cout << "RELEASED SPACE" << std::endl;
								player->setInteracting(false);
								player->setAction(Action::NONE);
//
						//if (player->getInteracting() && player->getOpeningBox()) {
						//	double seconds = player->getInteractingTime(0);
						//	if (seconds > gameData->getBoxTime()) {
						//		std::cout << "UNLOCKED BOX" << std::endl;
						//		gameData->getAtlas()->updateBoxLayout(loc);
						//		player->setInteracting(false);
						//		player->setOpeningBox(false);
// server
						//	}
						}

						/*if (player->isInteracting() &&
							!player->isChef()) {
							std::cout << "RELEASED SPACE" << std::endl;
							player->setInteracting(false);
// HEAD
						}*/


						//if (player->getOpeningGate() &&
						//	!player->isChef() && gameData->getAtlas()->hasGate(loc))
						//{
						//	GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
						//	if (!gateTile->isOpen())
						//	{
						//		std::cout << "RELEASED SPACE" << std::endl;
						//		player->setOpeningGate(false);

						//		//update progress of gate 
						//		double seconds = player->getInteractingTime(0);
						//		gateTile->constructGate(seconds);
						//	}
						//}
//
							player->setPoweringUp(false);
						}


						//if (player->getOpeningGate() &&
						//	!player->isChef() && gameData->getAtlas()->hasGate(loc))
						//{
						//	GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
						//	if (!gateTile->isOpen())
						//	{
						//		std::cout << "RELEASED SPACE" << std::endl;
						//		player->setOpeningGate(false);

						//		//update progress of gate 
						//		double seconds = player->getInteractingTime(0);
						//		gateTile->constructGate(seconds);
						//	}
						//}
// server
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

						std::cout << "facing:" << static_cast<int>(player->getFacingDirection()) << std::endl;
						Location loc = player->getLocation();

						// PLayer cannot drop item if there is an item already on the current tile
						if (!(gameData->getAtlas()->tileHasItem(loc)))
						{
							ItemModelType itemName = player->getInventory();
							gameData->getAtlas()->updateTileItem(loc, itemName);
							player->setInventory(ItemModelType::EMPTY);
							player->setSpeedMultiplier(1.0);

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
				std::cout << "RESET_EVENT" << std::endl;
				if (chefWin || animalWin) { 
					std::cout << "CALLING RESET" << std::endl;
					chefWin = false;
					animalWin = false;
					gameData->setWT(WinType::NONE);
					resetGame();
				}
				break;
			}

			default:
				printf("error in packet types\n");
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
			}
			
		}
		sendActionPackets(); // sends data after processing input from one clientss
		iter++;
	}
	gameData->getAtlas()->checkDroppedItems();
	if (gameData->getAtlas()->checkBoxRespawn())
	{
		sendActionPackets();
	}
	//sendActionPackets(); // uncomment to always send data from server
	if (gameData->countdownStarted() && !gameData->countdownDone())
	{
		sendActionPackets();
	}

	if (gameData->countdownDone())
	{
		gameStarted = true;
	}

	if (initCharacters)
	{
		initCharacters = false;
	}
	allPlayersReady = true;

	//All server loop checks 

	if (gameData->getGameClock() % gameData->getChefAngerInterval() == 0) {
		if (gameData->getGameClock() > gameData->getCurrentTime()) 
		{
			if (gameData->getChefAnger() < gameData->getMaxAnger())
			{
				gameData->incrementChefAnger();
			}
			if (gameData->getChefVision() < gameData->getChefMaxVision())
			{
				gameData->incrementChefVision();
			}
			gameData->setCurrentTime();
		}
	}

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		if (Player * player = gameData->getPlayer(iter->first))
		{
			if (!player->isReady())
			{
				allPlayersReady = false;
			}
			Location loc = player->getLocation();
			//std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };

// HEAD
			//if (player->isCaught())
//
			if (player->isChef()) {
				player->updateChefMultiplier(gameData->getChefAnger());
			}

			if (player->isCaught())
// server
			{
				continue;
			}

// HEAD
//			if (player->isInteracting())
//
			if (player->getBlindChef() && gameData->getGameClock() > player->getVisionTime() + MAX_CHEF_TIME)
			{
				player->toggleBlindChef();
				gameData->setChefVisionLimit(1);
			}

			if (player->getSlowChef() && gameData->getGameClock() > player->getSlowTime() + MAX_CHEF_TIME)
			{
				player->toggleSlowChef();
			}

			if (player->getGhost() && gameData->getGameClock() > player->getSpeedTime() + MAX_ANIMAL_GHOST_TIME)
			{
				player->toggleGhost();
			}

			if (player->getInstantSearch() && gameData->getGameClock() > player->getSearchTime() + MAX_ANIMAL_SEARCH_TIME)
			{
				player->toggleInstantSearch();
			}

			//if (player->getInteracting())
// server
			if (player->isInteracting())

			{
				double seconds = player->getInteractingTime(0);
				if (player->isChef())
				{
					if (seconds > SWING_NET_DELAY) {
						std::cout << "CAN SWING AGAIN" << std::endl;
						player->setInteracting(false);
						player->setAction(Action::NONE);
						sendActionPackets();
					}
				}
				else if (player->getAction() == Action::OPEN_BOX)
				{
// HEAD
					if (seconds > TIME_TO_OPEN_BOX) {
						std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
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
							//player->setOpenJail(false);
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
								std::cout << "GATE CONSTRUCTED" << std::endl;
								gateTile->constructGate(seconds);
								player->setInteracting(false);
								player->setAction(Action::NONE);
								sendActionPackets();
							}
						}
					}
				}
			
//
					/*if (player->getOpeningBox() && seconds > gameData->getBoxTime()) {
						std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
						gameData->getAtlas()->updateBoxLayout(loc);
						player->setInteracting(false);
						player->setOpeningBox(false);
					}
					std::cout << "Interacting!!" << std::endl;*/

					//Handling power ups
					if (player->getPoweringUp() && seconds > gameData->getAbilityChargeTime()) {
						std::cout << "POWERING UP" << std::endl;
						player->setInteracting(false);
						player->setPoweringUp(false);
						

						ItemModelType it = player->getInventory();
						if (it == ItemModelType::apple)
						{
							std::cout << "POWER: limit chef vision" << std::endl;
							//call limit chef vision
							player->toggleBlindChef();
							player->setVisionStartTime();
							gameData->setChefVisionLimit(0.4);
						}
						else if (it == ItemModelType::orange)
						{
							std::cout << "POWER: limit chef speed" << std::endl;
							//call limit chef speed 
							player->toggleSlowChef();
							player->setSlowStartTime();
						}
						else if (it == ItemModelType::bananaGreen)
						{
							std::cout << "POWER: ghost" << std::endl;
							player->toggleGhost();
							//call ghost because green means go 
							player->setSpeedStartTime();
						}
						else if (it == ItemModelType::bananaPerfect)
						{
							std::cout << "POWER: flash" << std::endl;

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
								z += FLASH_DISTANCE/1.5;
								x -= FLASH_DISTANCE/1.5;
							}
							else if (player->getFacingDirection() == Direction::NORTHWEST)
							{
								z += FLASH_DISTANCE/1.5;
								x+= FLASH_DISTANCE/1.5;
							}
							else if (player->getFacingDirection() == Direction::SOUTHEAST)
							{
								z -= FLASH_DISTANCE/1.5;
								x -= FLASH_DISTANCE/1.5;
							}
							else if (player->getFacingDirection() == Direction::SOUTHWEST)
							{
								z -= FLASH_DISTANCE/1.5;
								x += FLASH_DISTANCE/1.5;
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
							player->setLocation(x, y, z);
							sendActionPackets();
						}
						else if (it == ItemModelType::bananaVeryRipe)
						{
							std::cout << "POWER: search" << std::endl;
							player->toggleInstantSearch();
							//maybe like a trap item - makes chef slip or creates a barrier for a set duration
							player->setSearchStartTime();
						}
						player->setInventory(ItemModelType::EMPTY);
						player->setSpeedMultiplier(1.0);

// server
					}
			//	}
			//}

			/*if (player->getOpenJail()) 
			{
				double seconds = player->getInteractingTime(1);
				if (!player->isChef())
				{
					if (seconds > UNLOCK_JAIL_COOLDOWN) {
						player->setOpenJail(false);
					}
				}
			}*/

			/*if (gameData->getAtlas()->hasGate(loc))
			{
				GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));

				if (!gateTile->isOpen())
				{
					double seconds = player->getInteractingTime(0);

					if (player->getOpeningGate() && gateTile->getCurrentConstructTime() + seconds >= TIME_TO_CONSTRUCT_GATE)
					{
						std::cout << "GATE CONSTRUCTED" << std::endl;
						gateTile->constructGate(seconds);
					}
				}
// HEAD
			}*/
//
			}


// server
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
	//std::vector<int> loc{ 10, 0, 10 };

	gameData->addNewPlayer(client_id, Location(30, 0, 30), ClientType::SERVER_SIDE);
}

void ServerGame::updateMovement(int dir, int id)
{
	if (gameData->getPlayer(id)->isInteracting() ||
		gameData->getPlayer(id)->isCaught() ||
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

void ServerGame::updateRightEvent(int id)
{
	moveRight = true;
	Location loc = gameData->getPlayer(id)->getLocation();

	if (gameData->getPlayer(id)->isChef()) 
	{
		double multiplier = gameData->getPlayer(id)->getChefSpeedMultiplier();
		gameData->getPlayer(id)->setLocation(loc.getX() + (SPEED*multiplier), loc.getY(), loc.getZ());
	}
	else 
	{
		if (gameData->getPlayer(id)->getGhost()) 
		{
			gameData->getPlayer(id)->setLocation(loc.getX() + SPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER, loc.getY(), loc.getZ());
		}
		else 
		{
			gameData->getPlayer(id)->setLocation(loc.getX() + SPEED * gameData->getPlayer(id)->getSpeedMultiplier(), loc.getY(), loc.getZ());
		}
	}

	updatePlayerCollision(id, 0);
}

void ServerGame::updateBackwardEvent(int id)
{
	moveBackward = true;
	Location loc = gameData->getPlayer(id)->getLocation();

	if (gameData->getPlayer(id)->isChef())
	{
		double multiplier = gameData->getPlayer(id)->getChefSpeedMultiplier();
		gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() - (SPEED*multiplier));
	}
	else
	{
		if (gameData->getPlayer(id)->getGhost()) 
		{
			gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() - SPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER);
		}
		else
		{
			gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() - SPEED * gameData->getPlayer(id)->getSpeedMultiplier());
		}
	}

	updatePlayerCollision(id, 1);
}

void ServerGame::updateForwardEvent(int id)
{
	moveForward = true;
	Location loc = gameData->getPlayer(id)->getLocation();

	if (gameData->getPlayer(id)->isChef())
	{
		double multiplier = gameData->getPlayer(id)->getChefSpeedMultiplier();
		gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + (SPEED*multiplier));
	}
	else
	{
		if (gameData->getPlayer(id)->getGhost())
		{
			gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + SPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER);
		}
		else
		{
			gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + SPEED * gameData->getPlayer(id)->getSpeedMultiplier());
		}
	}
	updatePlayerCollision(id, 2);
	std::cout << "NORTH\n";

}

void ServerGame::updateLeftEvent(int id)
{
	moveLeft = true;
	Location loc = gameData->getPlayer(id)->getLocation();

	if (gameData->getPlayer(id)->isChef())
	{
		double multiplier = gameData->getPlayer(id)->getChefSpeedMultiplier();
		gameData->getPlayer(id)->setLocation(loc.getX() - (SPEED*multiplier), loc.getY(), loc.getZ());
	}
	else
	{
		if (gameData->getPlayer(id)->getGhost())
		{
			gameData->getPlayer(id)->setLocation(loc.getX() - SPEED * gameData->getPlayer(id)->getSpeedMultiplier() * GHOST_MULTIPLIER, loc.getY(), loc.getZ());
		}
		else 
		{
			gameData->getPlayer(id)->setLocation(loc.getX() - SPEED * gameData->getPlayer(id)->getSpeedMultiplier(), loc.getY(), loc.getZ());
		}
	}

	updatePlayerCollision(id, 3);
	std::cout << "WEST\n";
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

		return; 
	}

	//std::cout << "THERES A RAMP" << std::endl;

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

void ServerGame::updatePlayerCollision(int id, int dir) 
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
	std::cout << "CALLING RESET GAME" << std::endl;
	gameData = new GameData(SERVER_GAMEDATA);

	//reset players
	int count = 0;
	std::map<unsigned int, SOCKET>::iterator iter;
	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		Location loc = gameData->initLocs[count];
		gameData->addNewPlayer(iter->first, loc, ClientType::SERVER_SIDE);
		count++;
	}
	std::cout << "GAME RESET" << std::endl;

}
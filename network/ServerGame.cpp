#include "ServerGame.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include "Gate.h"

 
unsigned int ServerGame::client_id; 
unsigned int SPEED = 2;
bool chefWin = false;
bool animalWin = false;

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;
 
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

	if (chefWin || animalWin) { return; }
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
				gameData->removeClient(iter->first);
				network->sessions.erase(iter++);
			}
			else
			{
				iter++;
			}

			continue;
		}

		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			if (packet.packet_type != INIT_CONNECTION && packet.id == "") {
				continue;
			}

			switch (packet.packet_type) {
			case INIT_CONNECTION:
				printf("server received init packet from client\n");
				initNewClient();
				sendInitPackets();
				break;

			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				break;

			case FORWARD_EVENT:
				if (gameData->getPlayer(iter->first)->getInteracting()) { break; }
				updateForwardEvent(iter->first);
				updateCollision(iter->first);
				break;

			case BACKWARD_EVENT:
				if (gameData->getPlayer(iter->first)->getInteracting()) { break; }
				updateBackwardEvent(iter->first);
				updateCollision(iter->first);
				break;

			case LEFT_EVENT:
				if (gameData->getPlayer(iter->first)->getInteracting()) { break; }
				updateLeftEvent(iter->first);
				updateCollision(iter->first);
				break;

			case RIGHT_EVENT:
				if (gameData->getPlayer(iter->first)->getInteracting()) { break; }
				updateRightEvent(iter->first);
				updateCollision(iter->first);
				break;

			case INTERACT_EVENT:
			{

				Location loc = gameData->getPlayer(iter->first)->getLocation();
				//std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
				if (gameData->getPlayer(iter->first)->getIsChef()) 
				{
					if (gameData->getPlayer(iter->first)->getCaughtAnimal())
					{
						//drop off animal
						if (gameData->getAtlas()->hasJail(loc) && (gameData->getAtlas()->isJailEmpty(loc)))
						{
							//update jail with animal
							gameData->getAtlas()->placeInJail(loc);
							gameData->getPlayer(iter->first)->setCaughtAnimal();
						}
					}
					else
					{
						std::cout << "SWINGING" << std::endl;
						if (!gameData->getPlayer(iter->first)->getInteracting())
						{
							std::cout << "CAN SWING" << std::endl;
							gameData->getPlayer(iter->first)->setInteracting();
							gameData->getPlayer(iter->first)->setStartTime();
							std::map<unsigned int, SOCKET>::iterator iter2;
							for (iter2 = network->sessions.begin(); iter2 != network->sessions.end(); iter2++)
							{
								if (iter2 == iter || gameData->getPlayer(iter2->first)->getIsChef())
								{
									continue;
								}
								Location tLoc = gameData->getPlayer(iter2->first)->getLocation();
								//std::vector<float> theirLoc{ tLoc.getX(), tLoc.getY(), tLoc.getZ() };
								if (gameData->getPlayer(iter->first)->inRange(loc, tLoc))
								{
									gameData->getPlayer(iter->first)->setCaughtAnimal();
									gameData->getPlayer(iter2->first)->setIsCaught();
									break;
								}
							}
						}
					}
				}
				else
				{
					Location loc = gameData->getPlayer(iter->first)->getLocation();
					ItemName item = gameData->getAtlas()->getTileItem(loc);
					
					if (item != ItemName::EMPTY && gameData->getPlayer(iter->first)->getInventory() == ItemName::EMPTY)
					{
						gameData->getPlayer(iter->first)->setInventory(item);
						gameData->getAtlas()->updateTileItem(loc, ItemName::EMPTY);

					}
					else if (gameData->getAtlas()->hasGate(loc))
					{
						std::cout << "HAS GATE" << std::endl;
						if (gameData->getGate().isValidKey(static_cast<Key>(gameData->getPlayer(iter->first)->getInventory())))
						{
							std::cout << "UPDATING GATE PROGRESS" << std::endl;
							gameData->getGate().updateProgress(static_cast<Key>(gameData->getPlayer(iter->first)->getInventory()));
						}
						if (gameData->getGate().getHasKeys() && !gameData->getPlayer(iter->first)->getOpenGate())
						{
							gameData->getPlayer(iter->first)->setOpenGate();
							gameData->getPlayer(iter->first)->setStartTime();
						}
					}
					else if (gameData->getAtlas()->hasJail(loc))
					{
						std::cout << "HAS JAIL" << std::endl;
						if (!gameData->getAtlas()->isJailEmpty(loc))
						{
							gameData->getPlayer(iter->first)->setOpenJail();
							gameData->getPlayer(iter->first)->setStartJailTime();
							
							//update jail progress
							gameData->getAtlas()->unlockJail(loc);

							//check if jail progress == 5
							if (gameData->getAtlas()->getJailProgress(loc) >= 5)
							{
								//update animal 
								std::map<unsigned int, SOCKET>::iterator iter2;
								for (iter2 = network->sessions.begin(); iter2 != network->sessions.end(); iter2++)
								{
									if (iter2 == iter)
									{
										continue;
									}
									Location tLoc = gameData->getPlayer(iter2->first)->getLocation();
									std::vector<float> theirLoc{ tLoc.getX(), tLoc.getY(), tLoc.getZ() };
									if (loc == tLoc && gameData->getPlayer(iter2->first)->getIsCaught())
									{
										gameData->getPlayer(iter2->first)->setIsCaught();
									}
								}

								//update jail
								gameData->getAtlas()->resetJail(loc);
							}
						}
					}
					else if (gameData->getAtlas()->hasBox(loc))
					{
						std::cout << "HAS BOX" << std::endl;
						if (!gameData->getPlayer(iter->first)->getInteracting()) {
							std::cout << "starting to interact!" << std::endl;
							gameData->getPlayer(iter->first)->setInteracting();
							gameData->getPlayer(iter->first)->setStartTime();
						}
					}
				}
				break;
			}

			case RELEASE_EVENT:
			{
				if (gameData->getPlayer(iter->first)->getIsCaught()) {
					break;
				}

				Location loc = gameData->getPlayer(iter->first)->getLocation();

				if (gameData->getPlayer(iter->first)->getInteracting()) {
					double seconds = gameData->getPlayer(iter->first)->checkProgress(0);
					if (seconds > gameData->getBoxTime()) {
						std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
						gameData->getAtlas()->updateBoxLayout(loc);
						gameData->getPlayer(iter->first)->setInteracting();
					}
				}

				if (gameData->getPlayer(iter->first)->getInteracting() && 
					!gameData->getPlayer(iter->first)->getIsChef()) {
					std::cout << "RELEASED SPACE" << std::endl;
					gameData->getPlayer(iter->first)->setInteracting();
				}

				if (gameData->getPlayer(iter->first)->getOpenGate() &&
					!gameData->getPlayer(iter->first)->getIsChef() &&
					!gameData->getGate().getIsOpen()) {
					std::cout << "RELEASED SPACE" << std::endl;
					gameData->getPlayer(iter->first)->setOpenGate();

					//update progress of gate 
					double seconds = gameData->getPlayer(iter->first)->checkProgress(0);
					gameData->getGate().constructGate(seconds);
				}
				break;
			}
			case DROP_EVENT:
			{
				Location loc = gameData->getPlayer(iter->first)->getLocation();

				// PLayer cannot drop item if there is an item already on the current tile
				if (!(gameData->getAtlas()->tileHasItem(loc)))
				{
					ItemName itemName = gameData->getPlayer(iter->first)->getInventory();
					gameData->getAtlas()->updateTileItem(loc, itemName);
					gameData->getPlayer(iter->first)->setInventory(ItemName::EMPTY);

					gameData->getAtlas()->updateDroppedItem(itemName, loc);
					/*Item temp;
					gameData->getAtlas()->getItem(itemName, temp);
					if (temp.getName() != ItemName::EMPTY)
					{
						temp.setHoldStatus(false);
						int row, col;
						Atlas::getMapCoords(loc, row, col);
						if (temp.hasBeenMoved(row, col))
						{
							temp.setDropped(true);
							temp.setDroppedTime(clock());
						}
					}*/
				}
				break;
			}
			default:
				printf("error in packet types\n");
				break;
			}
		}
		sendActionPackets(); // sends data after processing input from one clientss
		iter++;
	}
	gameData->getAtlas()->checkDroppedItems();
	//sendActionPackets(); // uncomment to always send data from server
	chefWin = true;
	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		if (gameData->getPlayer(iter->first)) {
			Location loc = gameData->getPlayer(iter->first)->getLocation();
			//std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };

			if (!gameData->getPlayer(iter->first)->getIsChef() &&
				!gameData->getPlayer(iter->first)->getIsCaught()) 
			{
				chefWin = false;
			}

			if (gameData->getPlayer(iter->first)->getIsCaught())
			{
				continue;
			}

			if (gameData->getPlayer(iter->first)->getInteracting())
			{
				double seconds = gameData->getPlayer(iter->first)->checkProgress(0);
				if (gameData->getPlayer(iter->first)->getIsChef())
				{
					if (seconds > gameData->getChefSwingTime()) {
						std::cout << "CAN SWING AGAIN" << std::endl;
						gameData->getPlayer(iter->first)->setInteracting();
					}
				}
				else
				{
					if (seconds > gameData->getBoxTime()) {
						std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
						gameData->getAtlas()->updateBoxLayout(loc);
						gameData->getPlayer(iter->first)->setInteracting();
					}
				}
			}

			if (gameData->getPlayer(iter->first)->getOpenJail()) 
			{
				double seconds = gameData->getPlayer(iter->first)->checkProgress(1);
				if (!gameData->getPlayer(iter->first)->getIsChef())
				{
					if (seconds > gameData->getOpenJailTime()) {
						std::cout << "CAN OPEN AGAIN" << std::endl;
						gameData->getPlayer(iter->first)->setOpenJail();
					}
				}
			}

			/*if (gameData->getAtlas()->hasGate(loc) && !gameData->getGate().getIsOpen()) 
			{
				double seconds = gameData->getPlayer(iter->first)->checkProgress(0);
				if (seconds + gameData->getGate().getTotalConstructTime() >
					gameData->getGate().getFinishTime())
				{
					gameData->getGate().setOpen();
					gameData->getGate().constructGate(seconds);
				}
			}*/
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
	std::string msg_string = gameData->encodeGameData();
	
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

	gameData->addNewClient(client_id, Location(30, 0, 30));
}

void ServerGame::updateRightEvent(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX() + SPEED, loc.getY(), loc.getZ());
	updatePlayerCollision(id, 0);
}

void ServerGame::updateBackwardEvent(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() - SPEED);
	updatePlayerCollision(id, 1);
}

void ServerGame::updateForwardEvent(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + SPEED);
	updatePlayerCollision(id, 2);
}

void ServerGame::updateLeftEvent(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX() - SPEED, loc.getY(), loc.getZ());
	updatePlayerCollision(id, 3);
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

void ServerGame::updateCollision(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	//std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	gameData->getAtlas()->detectCollision(loc);
	//gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
	gameData->getPlayer(id)->setLocation(loc);
}
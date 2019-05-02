#include "ServerGame.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>

 
unsigned int ServerGame::client_id; 
unsigned int SPEED = 2;

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
				updateForwardEvent(iter->first);
				updateCollision(iter->first);
				break;

			case BACKWARD_EVENT:
				updateBackwardEvent(iter->first);
				updateCollision(iter->first);
				break;

			case LEFT_EVENT:
				updateLeftEvent(iter->first);
				updateCollision(iter->first);
				break;

			case RIGHT_EVENT:
				updateRightEvent(iter->first);
				updateCollision(iter->first);
				break;

			case INTERACT_EVENT:
			{
				Location pLoc = gameData->getPlayer(iter->first)->getLocation();
				std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
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
								std::vector<float> theirLoc{ tLoc.getX(), tLoc.getY(), tLoc.getZ() };
								if (gameData->getPlayer(iter->first)->inRange(loc, theirLoc))
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
					if (int key = gameData->getAtlas()->hasKey(loc))
					{
						gameData->getPlayer(iter->first)->setInventory(static_cast<ItemName>(key));
					}
					else if (gameData->getAtlas()->hasGate(loc))
					{
						if (gameData->getGate().isValidKey(static_cast<Key>(gameData->getPlayer(iter->first)->getInventory())))
						{
							gameData->getGate().updateProgress(static_cast<Key>(gameData->getPlayer(iter->first)->getInventory()));
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

				if (gameData->getPlayer(iter->first)->getInteracting() && 
					!gameData->getPlayer(iter->first)->getIsChef()) {
					std::cout << "RELEASED SPACE" << std::endl;
					gameData->getPlayer(iter->first)->setInteracting();
				}
				break;
			}
			case DROP_EVENT:
			{
				gameData->getPlayer(iter->first)->setInventory(ItemName::EMPTY);
			}
			default:
				printf("error in packet types\n");
				break;
			}
		}
		sendActionPackets(); // sends data after processing input from one clientss
		iter++;
	}
	//sendActionPackets(); // uncomment to always send data from server
	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		if (gameData->getPlayer(iter->first)) {
			Location pLoc = gameData->getPlayer(iter->first)->getLocation();
			std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };

			if (gameData->getPlayer(iter->first)->getIsCaught())
			{
				continue;
			}

			if (gameData->getPlayer(iter->first)->getInteracting())
			{
				double seconds = gameData->getPlayer(iter->first)->checkProgress();
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
	Location pLoc = gameData->getPlayer(id)->getLocation();
	std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	gameData->getAtlas()->detectCollision(loc);
	gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
}
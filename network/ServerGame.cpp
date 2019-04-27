#include "ServerGame.h"
#include "Walls.h"
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
	walls = new Walls();
	gameData = new GameData(walls);
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
				std::cout << "interact received\n";
				break;
			default:
				printf("error in packet types\n");
				break;
			}
			sendActionPackets(); // sends data after processing input from one client
		}
		iter++;
	}
	//sendActionPackets(); // uncomment to always send data from server
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
		
		if (dist < 2 * Walls::PLAYER_RADIUS) 
		{
			if (dir == 0) 
			{
				loc[0] = ot_x - 2 * Walls::PLAYER_RADIUS;
			} 
			else if (dir == 1)
			{
				loc[2] = ot_z - 2 * Walls::PLAYER_RADIUS;
			}
			else if (dir == 2)
			{
				loc[2] = ot_z + 2 * Walls::PLAYER_RADIUS;
			}
			else
			{
				loc[0] = ot_x + 2 * Walls::PLAYER_RADIUS;
			}
			gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
		}
	}
}

void ServerGame::updateCollision(int id)
{
	Location pLoc = gameData->getPlayer(id)->getLocation();
	std::vector<float> loc{ pLoc.getX(), pLoc.getY(), pLoc.getZ() };
	walls->detectCollision(loc);
	gameData->getPlayer(id)->setLocation(loc[0], loc[1], loc[2]);
}
#include "ServerGame.h"
#include "Walls.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>



std::map < std::string, vector<int>> clients;
 
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
 
    // get new clients
    if(network->acceptNewClient(client_id))
    {
        printf("client %d has been connected to the server\n",client_id); 
		//initNewClient();
    }

	receiveFromClients();
	sendActionPackets();
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


			//std::string clientid;

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

				//sendActionPackets();

				break;

			case FORWARD_EVENT:

				//printf("Forward event called\n");
				cout << "this is network data" << packet.id << endl;
				//updateForwardEvent(std::string(packet.id));
				updateForwardEvent2(iter->first);
				//updateCollision(std::string(packet.id));

				//sendActionPackets();

				break;

			case BACKWARD_EVENT:

				updateBackwardEvent(std::string(packet.id));
				updateCollision(std::string(packet.id));

				//sendActionPackets();

				break;

			case LEFT_EVENT:

				updateLeftEvent(std::string(packet.id));
				updateCollision(std::string(packet.id));

				//sendActionPackets();

				break;

			case RIGHT_EVENT:


				//printf("Right event called\n");

				updateRightEvent(std::string(packet.id));
				updateCollision(std::string(packet.id));


				//sendActionPackets();

				break;

			default:

				printf("error in packet types\n");

				break;
			}
		}
		iter++;
	}
}

void ServerGame::sendInitPackets()
{
	std::string msg_string = "client_" + std::to_string(client_id) + "\n";
	msg_string += "init:client_" + std::to_string(client_id) + "\n";

	msg_string += "-----\n";
	
	msg_string = "init: " + std::to_string(client_id) + "\n";
	client_id++;

	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	int i;
	for (i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	//printf("sendtoall\n");

	network->sendToAll(msg, packet_size);
	delete[] msg;
}

void ServerGame::sendActionPackets()
{

	//READ IN DATA STRUCTURES AND SEND DATA ACCORDINGLY
	std::string msg_string = "";
	for (auto const& x : clients)
	{
		msg_string += x.first + "\n";
		msg_string += "location:" + std::to_string(x.second[0]) + std::string(" ") + std::to_string(x.second[1]) + std::string(" ") + std::to_string(x.second[2]) + std::string("\n");
	}
	msg_string += "-----\n";
	
	// gets an encoded game data string for all players and general game data
	msg_string = gameData->encodeGameData(); 
	
	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	int i;
	for (i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	//printf("sendtoall\n");

	network->sendToAll(msg, packet_size);
	delete[] msg;
}


void ServerGame::initNewClient()
{
	//updating current data structure to hold onto client
	std::vector<int> loc{ 10, 0, 10 };
	std::string id = "client_" + std::to_string(client_id);
	clients[id] = loc;

	gameData->addNewClient(client_id);
	/*
	std::string msg_string = "SERVER INITIALIZATION\n";

	msg_string += "id:                " + id;
	msg_string += "\n-----";

	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	int i;
	for (i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];

	}
	network->sendToClient(msg, packet_size, client_id);
	client_id++;
	*/
}

void ServerGame::updateRightEvent(std::string id)
{
	clients[id][0] += SPEED;
	updatePlayerCollision(id, 0);
}

void ServerGame::updateBackwardEvent(std::string id)
{
	clients[id][2] -= SPEED;
	updatePlayerCollision(id, 1);
}

void ServerGame::updateForwardEvent(std::string id)
{
	clients[id][2] += SPEED;
	updatePlayerCollision(id, 2);
}

void ServerGame::updateForwardEvent2(int id)
{
	Location loc = gameData->getPlayer(id)->getLocation();
	gameData->getPlayer(id)->setLocation(loc.getX(), loc.getY(), loc.getZ() + SPEED);
	//updatePlayerCollision2(id, 2);
}

void ServerGame::updateLeftEvent(std::string id)
{
	clients[id][0] -= SPEED;
	updatePlayerCollision(id, 3);
}



void ServerGame::updatePlayerCollision(std::string id, int dir) 
{
	std::vector<int> loc = clients[id];

	//0 == right
	//1 == down
	//2 == forward
	//3 == left
	map<string, vector<int>>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (it->first == id) {
			continue;
		}

		//calculate distance between two points 
		int my_x = clients[id][0];
		int my_z = clients[id][2];

		int ot_x = it->second[0];
		int ot_z = it->second[2];

		float dist = sqrt(pow(my_x - ot_x, 2) + pow(my_z - ot_z, 2) * 1.0);
		
		if (dist < 2 * Walls::PLAYER_RADIUS) 
		{
			if (dir == 0) 
			{
				clients[id][0] = ot_x - 2 * Walls::PLAYER_RADIUS;
			} 
			else if (dir == 1)
			{
				clients[id][2] = ot_z - 2 * Walls::PLAYER_RADIUS;
			}
			else if (dir == 2)
			{
				clients[id][2] = ot_z + 2 * Walls::PLAYER_RADIUS;
			}
			else
			{
				clients[id][0] = ot_x + 2 * Walls::PLAYER_RADIUS;
			}
		}
	}
}

//void ServerGame::updatePlayerCollision2(int id, int dir)


void ServerGame::updateCollision(std::string id)
{
	std::vector<int> loc = clients[id];
	walls->detectCollision(loc);
}
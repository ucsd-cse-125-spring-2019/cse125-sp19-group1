#include "ServerGame.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>



std::map < std::string, vector<int>> clients;
 
unsigned int ServerGame::client_id; 

unsigned int SPEED = 10;


ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;
 
    // set up the server network to listen 
    network = new ServerNetwork(); 
}
 
void ServerGame::update() 
{
 
    // get new clients
    if(network->acceptNewClient(client_id))
    {
        printf("client %d has been connected to the server\n",client_id); 
		initNewClient();
    }

	receiveFromClients();
}

void ServerGame::receiveFromClients()
{

	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		int data_length = network->receiveData(iter->first, network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type) {

			case INIT_CONNECTION:

				printf("server received init packet from client\n");

				initNewClient();

				break;

			case ACTION_EVENT:

				printf("server received action event packet from client\n");

				sendActionPackets();

				break;

			case FORWARD_EVENT:

				//printf("Forward event called\n");

				updateForwardEvent("client_1");

				sendActionPackets();

				break;

			case BACKWARD_EVENT:

				printf("Backwards event called\n");

				sendActionPackets();

				break;

			case LEFT_EVENT:

				printf("Left event called\n");

				sendActionPackets();

				break;

			case RIGHT_EVENT:

				printf("Right event called\n");
				
				sendActionPackets();

				break;

			default:

				printf("error in packet types\n");

				break;
			}
		}
	}
}
void ServerGame::sendActionPackets()
{
	//READ IN DATA STRUCTURES AND SEND DATA ACCORDINGLY
	std::string msg_string = "";
	for (auto const& x : clients)
	{
		msg_string += x.first + "\n";
		msg_string += "location:"+x.second[0]+" "+ x.second[1]+" "+ x.second[2]+"\n"
	}
	msg_string += "-----"

	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	int i;
	for (i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	network->sendToAll(msg, packet_size);
	delete[] msg;
}


void ServerGame::initNewClient()
{
	//updating current data structure to hold onto client
	std::vector<int> loc{ 0, 0, 0 };
	std::string id = "client_" + std::to_string(client_id);
	clients[id] = loc;
	client_id++;
}

void ServerGame::updateForwardEvent(std::string id)
{
	clients[id][2] +=  SPEED;
}

void ServerGame::updateBackwardEvent(std::string id)
{
	clients[id][2] -= SPEED;
}

void ServerGame::updateLeftEvent(std::string id)
{
	clients[id][0] -= SPEED;
}

void ServerGame::updateRightEvent(std::string id)
{
	clients[id][0] += SPEED;
}
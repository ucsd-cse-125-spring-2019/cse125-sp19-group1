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

				//sendActionPackets();

				break;

			case FORWARD_EVENT:

				printf("Forward event called\n");

				updateForwardEvent("client_1");

				break;

			case BACKWARD_EVENT:

				printf("Backwards event called\n");

				updateBackwardEvent("client_1");

				break;

			case LEFT_EVENT:

				printf("Left event called\n");

				updateLeftEvent("client_1");

				break;

			case RIGHT_EVENT:

				printf("Right event called\n");
				
				updateRightEvent("client_1");

				break;

			default:

				printf("error in packet types\n");

				break;
			}
		}
	}
}
void ServerGame::sendActionPackets(std::string id, std::string data)
{
	// send action packet
	//const unsigned int packet_size = sizeof(Packet);

	//const unsigned int packet_size = 100;
	//char packet_data[packet_size] = "{ \"a\": \"b\"}";

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	char text[100] = "hello world";


	packet.databuf = 'h';

	//packet.serialize(packet_data);

	//char packet_data[packet_size];
	//const unsigned int packet_size = 100;

	//std::cout << "id: " << id << std::endl;
	//std::cout << "data: " << data << std::endl;

	std::string msg_string = "" + id + "\n" + data + "\r\n";
	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	int i;
	for (i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	//packet.serialize(msg);

	//packet.databuf = 'h';
	//printf(msg);
	
	//char packet_data[packet_size] = 

	//Packet packet;
	//packet.packet_type = ACTION_EVENT;
	//char text[100] = "hello world";

	//packet.serialize(packet_data);
	//packet.databuf = 'h';

	//network->sendToAll(packet_data, packet_size);
	printf("sendtoall\n");
	network->sendToAll(msg, packet_size);
	delete[] msg;
}


void ServerGame::initNewClient()
{
	//updating current data structure to hold onto client
	std::vector<int> loc{ 10, 20, 30 };
	std::string id = "client_" + std::to_string(client_id);
	clients[id] = loc;
	client_id++;

	std::string data =
		"location: " + std::to_string(loc[0]) + " " +
		"" + std::to_string(loc[1]) + " " +
		"" + std::to_string(loc[2]) + "\n";

	//send to client that connected their current id
	//update all other clients that a new client has joined
	sendActionPackets(id, data);
}

void ServerGame::updateForwardEvent(std::string id)
{
	clients[id][2] +=  SPEED;
	vector<int> loc = clients[id];
		
	std::string data =
		"location: " + std::to_string(loc[0]) + " " +
		"" + std::to_string(loc[1]) + " " +
		"" + std::to_string(loc[2]) + "\n";

	sendActionPackets(id, data);
}

void ServerGame::updateBackwardEvent(std::string id)
{
	clients[id][2] -= SPEED;
	vector<int> loc = clients[id];

	std::string data =
		"x:" + std::to_string(loc[0]) + ";" +
		"y:" + std::to_string(loc[1]) + ";" +
		"z:" + std::to_string(loc[2]);

	sendActionPackets(id, data);
}

void ServerGame::updateLeftEvent(std::string id)
{
	vector<int> loc = clients[id];
	clients[id][0] -= SPEED;

	std::string data =
		"x:" + std::to_string(loc[0]) + ";" +
		"y:" + std::to_string(loc[1]) + ";" +
		"z:" + std::to_string(loc[2]);

	sendActionPackets(id, data);
}

void ServerGame::updateRightEvent(std::string id)
{
	vector<int> loc = clients[id];
	clients[id][0] += SPEED;

	std::string data =
		"x:" + std::to_string(loc[0]) + ";" +
		"y:" + std::to_string(loc[1]) + ";" +
		"z:" + std::to_string(loc[2]);


	sendActionPackets(id, data);
}
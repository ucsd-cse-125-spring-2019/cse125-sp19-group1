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
		//initNewClient();
    }

	receiveFromClients();
}

void ServerGame::receiveFromClients()
{

	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;
	try
	{
		for (iter = network->sessions.begin(); iter != network->sessions.end(); )//iter++)
		{
			int data_length = network->receiveData(iter->first, network_data);
			cout << "size:" << network->sessions.size() << std::endl;
			if (data_length <= 0)
			{
				//no data recieved
				if (network->sessions.size() == 0)
					break;

				if (data_length == 0 ||( data_length == -1 && WSAGetLastError() == 10054))
				{
					printf("Client disconnected\n");
					closesocket(iter->second);
					if (network->sessions.size() == 1)
					{


						network->sessions.erase(iter);
						break;
					}
					else
					{
						if (network->sessions.size() == 0)
							break;

						network->sessions.erase(iter++);

					}

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

				std::string clientid;
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
					updateForwardEvent("client_" + std::to_string(iter->first));

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
			iter++;
		}
		
	}
	catch (...)
	{
		printf("ERRORS HERE");
	}
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
	msg_string += "-----";

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
	std::vector<int> loc{ 0, 0, 0 };
	std::string id = "client_" + std::to_string(client_id);
	clients[id] = loc;


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
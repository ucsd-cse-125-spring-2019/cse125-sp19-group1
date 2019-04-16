#include "ClientGame.h"
#include <iostream>
#include <map>
#include <vector>

//std::map < std::string, std::vector<int>> clients;

//initialization 
ClientGame::ClientGame(void)
{
	network = new ClientNetwork();

	// send init packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//Sending packet information
void ClientGame::sendActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendForwardPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = FORWARD_EVENT;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//Getting data back and updating game state 
void ClientGame::update()
{
	Packet packet;
	int data_length = network->receivePackets(network_data);

	if (data_length <= 0)
	{
		//no data recieved
		return;
	}
	std::cout << "data received: " << network_data << std::endl;
	decodeData(network_data);
}

void ClientGame::decodeData(const char * data)
{
	std::string dataIn(data);
	std::stringstream strStream(dataIn);
	std::istream &dataStream(strStream);

	std::string clientID = "";
	std::string key_str = "";
	std::string value_str = "";

	while (dataStream.peek() != EOF)
	{
		std::string token_str = "";
		char c = dataStream.peek();

		if (c == '-')
		{
			while (c == '-')
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}
			clientID.clear();
		}

		if (isalpha(c)) // read word
		{
			while (isalpha(c) || isdigit(c) || c == '_')
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}

			if (clientID.empty())
			{
				clientID = token_str;
			}

			if (token_str == "type")
			{
				key_str.clear();
				while (c != ',')
				{
					key_str += dataStream.get();
					c = dataStream.peek();
				}

				key_str.erase(0, 2);
				//std::cout << field << ":" << type_str << std::endl;

			}


			if (token_str == "value")
			{
				value_str.clear();
				while (c != '\n')
				{
					value_str += dataStream.get();
					c = dataStream.peek();

				}

				value_str.erase(0, 2);
				value_str.erase(value_str.length() - 2, 2);
				std::cout << clientID << "--" << key_str << "--" << value_str << std::endl;

			}

			//std::cout << field << ":" << token_str << std::endl;

		}
		else // Remove whitespace/miscellanous chars
		{
			c = dataStream.get();
		}
	}
	std::cout << "decode:" << std::endl;
	std::cout << data << std::endl;
}
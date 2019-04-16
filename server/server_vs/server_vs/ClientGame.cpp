#include "ClientGame.h"
#include <iostream>
#include <map>
#include <vector>

std::map < std::string, std::vector<int> > clients2;

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

	std::cout << "data received:\n" << network_data << std::endl;
	decodeData(network_data);

	// printing out client data for debug checking
	std::string msg_string;
	for (auto const& x : clients2)
	{
		msg_string += x.first + "\n";
		msg_string += "location:" + std::to_string(x.second[0]) + std::string(" ") + std::to_string(x.second[1]) + std::string(" ") + std::to_string(x.second[2]) + std::string("\n");
	}

	std::cout << "msgstr:\n" << msg_string << std::endl;
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

			// reset client ID at end of block
			if(token_str == "-----")
				clientID.clear();
		}

		// Clear key and values at end of line
		if (c == '\n')
		{
			key_str.clear();
			value_str.clear();
		}

		if (isalpha(c) || isdigit(c)) // read word
		{
			while ((isalpha(c) || isdigit(c) || c == '_') || (!key_str.empty() && c != '\n'))
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}

			if (clientID.empty())
			{
				clientID = token_str;
			}
			else if (key_str.empty())
			{
				key_str = token_str;

			}
			else
			{
				value_str = token_str;

				if (key_str == "location")
				{

					std::vector<int> vec3;
					std::stringstream valStrStream(value_str);
					std::istream &valStream(valStrStream);
					
					while (valStream.peek() != EOF)
					{
						std::string val_str = "";
						char d = valStream.peek();

						if (isdigit(d) || d == '.')
						{
							while (isdigit(d))
							{
								val_str += valStream.get();
								d = valStream.peek();
							}

							vec3.push_back(std::stoi(val_str));
							std::cout << "num: " << val_str << std::endl;
						}
						else
						{
							d = valStream.get(); // removes whitespace, etc
						}
					}

					clients2[clientID] = vec3;
				}
			}
			std::cout << "clientID: " << clientID << "  key: " << key_str << "  value: " << value_str << std::endl;
		}
		else // Remove whitespace/miscellanous chars
		{
			c = dataStream.get();
		}

		
	}
	std::cout << "\ndecode:" << std::endl;
	std::cout << data << std::endl;

}
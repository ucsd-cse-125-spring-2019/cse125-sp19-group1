#include "ClientGame.h"

//initialization 
ClientGame::ClientGame(void)
{
	gameData = new GameData();
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

void ClientGame::sendMovementPackets(const int type)
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = type;

	if (my_client_id == "") {
		return;
	}

	std::cout << "About to send movement packet with ID: " << my_client_id << std::endl;
	memset(packet.id, 0, sizeof(packet.id));
	int i;
	for (i = 0; i < my_client_id.size() && i < 8; i++) {
		packet.id[i] = my_client_id[i];
	}
	//packet.id[8] = '\0';


	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//Getting data back and updating game state 
void ClientGame::update()
{
	int data_length = network->receivePackets(network_data);

	if (data_length <= 0)
	{
		//no data recieved
		return;
	}

	std::cout << "data received on client:\n" << network_data << std::endl;

	if (myID == -1)
	{
		std::vector<std::pair<std::string, std::string>> keyValuePairs;
		keyValuePairs = StringParser::parseKeyValueString(network_data);
		std::string key = keyValuePairs[0].first;
		std::string value = keyValuePairs[0].second;

		if (key == "init")
			myID = std::stoi(value);
	}
	else
	{
		gameData->decodeGameData(network_data);
	}
	decodeData(network_data);

	// printing out client data for debug checking
	std::string msg_string;
	for (auto const& x : clients2)
	{
		msg_string += x.first + "\n";
		msg_string += "location:" + std::to_string(x.second[0]) + std::string(" ") + std::to_string(x.second[1]) + std::string(" ") + std::to_string(x.second[2]) + std::string("\n");
	}

	//std::cout << "after parse:\n" << msg_string << std::endl;
	memset(network_data, 0, sizeof(network_data)); // empties the buffer after parsing
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

			// reset client ID
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

				if (key_str == "init" && my_client_id == "") {
					my_client_id = token_str;
					std::cout << "My client ID: " << my_client_id << std::endl;
					std::vector<float> vector(3, 0.0);
					clients2[my_client_id] = vector;
				}

				else if (key_str == "location")
				{

					std::vector<float> vec3;
					std::stringstream valStrStream(value_str);
					std::istream &valStream(valStrStream);
					
					while (valStream.peek() != EOF)
					{
						std::string val_str = "";
						char d = valStream.peek();

						if (isdigit(d) || d == '.' || d == '-')
						{
							while (isdigit(d) || d == '.' || d == '-')
							{
								val_str += valStream.get();
								d = valStream.peek();
							}


							//vec3.push_back(std::stof(val_str));
							//std::cout << "num: " << val_str << std::endl;

							vec3.push_back(val_str.size() == 0 ? 0 : std::stoi(val_str));
							//std::cout << "num: " << val_str << std::endl;
						}
						else
						{
							d = valStream.get(); // removes whitespace, etc
						}
					}

					clients2[clientID] = vec3;
					allClients[clientID] = Player(0, Location(vec3[0], vec3[1], vec3[2]));
				}
			}
			//std::cout << "clientID: " << clientID << "  key: " << key_str << "  value: " << value_str << std::endl;
		}
		else // Remove whitespace/miscellanous chars
		{
			c = dataStream.get();
		}

		
	}
	//std::cout << "\ndecode:" << std::endl;
	//std::cout << data << std::endl;

}

GameData * ClientGame::getGameData()
{
	return gameData;
}

int ClientGame::getMyID()
{
	return myID;
}


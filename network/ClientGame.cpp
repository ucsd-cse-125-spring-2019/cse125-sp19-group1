#include "ClientGame.h"

#include <fstream>

// Comment this out to print all messages to stdout, even messages larger than 128 chars
#define CENSOR_LARGE_MSG 128

void loadMapArray(std::vector<std::vector<uint8_t>> &array, const char *filepath) {
	std::ifstream inf(filepath);

	if (!inf.is_open()) {
		throw new std::invalid_argument("could not open filepath");
	}

	int width, height;
	inf >> width >> height;

	array.resize(height);
	for (auto &v : array) {
		v.resize(width);

		for (auto &elim : v) {
			unsigned x;
			inf >> x;
			elim = x;
		}
	}

	inf.close();
}

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
	packet.id = myID;
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
	packet.id = myID;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendPackets(const int type, const int selectionNum)
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = type;
	packet.id = myID;
	packet.selectionNum = selectionNum;

	if (myID == -1) {
		return;
	}

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

#ifdef CENSOR_LARGE_MSG
	const auto len = strlen(network_data);
	if (len > CENSOR_LARGE_MSG)
		std::cout << "data received on client: (censored because its " << len << " bytes)" << std::endl;
	else
#endif
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
	//else
	//{
		gameData->decodeGameData(network_data);
	//}

	// empties the buffer after parsing
	memset(network_data, 0, sizeof(network_data)); 
}

GameData * ClientGame::getGameData()
{
	return gameData;
}

int ClientGame::getMyID()
{
	return myID;
}


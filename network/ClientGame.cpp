#include "ClientGame.h"
#include "../graphics/GraphicsClient/SoundSystem.h"

#include <fstream>

// Comment this out to print all messages to stdout, even messages larger than 128 chars
#define CENSOR_LARGE_MSG 128

// Paths for sounds
#define SOUNDS_PATH			"../../sounds/"
#define SOUNDS_DOOR			(SOUNDS_PATH "frontexit_door_short.mp3")
#define SOUNDS_DOOR_UNLOCK	(SOUNDS_PATH "frontexit_unlock_short.mp3")
#define SOUNDS_FOUND_ITEM	(SOUNDS_PATH "found_item_short.mp3")
#define SOUNDS_NET			(SOUNDS_PATH "pop.mp3")	// FIXME placeholder for swoosh
#define SOUNDS_RACCOON_UP	(SOUNDS_PATH "RaccoonDownUp.mp3")
#define SOUNDS_RACCOON_DOWN	(SOUNDS_PATH "RaccoonUpDown.mp3")
#define SOUNDS_SEARCH_ITEM	(SOUNDS_PATH "search_item.mp3")
#define SOUNDS_TOILET		(SOUNDS_PATH "bathroom_toilet.mp3")
#define SOUNDS_VENT_SCREW	(SOUNDS_PATH "ventexit_screw.mp3")
#define SOUNDS_WINDOW		(SOUNDS_PATH "bathroom_window.mp3")
#define SOUNDS_YAY			(SOUNDS_PATH "Yay.mp3")
#define SOUNDS_JAIL_UNLOCK	(SOUNDS_PATH "jail_rattle.mp3")

static SoundSystem * soundSystem;
static Sound * sound_door;
static Sound * sound_door_unlock;
static Sound * sound_found_item;
static Sound * sound_net;
static Sound * sound_raccoon_up;
static Sound * sound_raccoon_down;
static Sound * sound_search_item;
static Sound * sound_toilet;
static Sound * sound_vent_screw;
static Sound * sound_window;
static Sound * sound_yay;
static Sound * sound_jail_unlock;

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

	soundSystem = new SoundSystem();
	if (!(soundSystem->shouldIgnoreSound())) {
		soundSystem->createSound(&sound_door, SOUNDS_DOOR);
		soundSystem->createSound(&sound_door_unlock, SOUNDS_DOOR_UNLOCK);
		soundSystem->createSound(&sound_found_item, SOUNDS_FOUND_ITEM);
		soundSystem->createSound(&sound_net, SOUNDS_NET);
		soundSystem->createSound(&sound_raccoon_up, SOUNDS_RACCOON_UP);
		soundSystem->createSound(&sound_raccoon_down, SOUNDS_RACCOON_DOWN);
		soundSystem->createSound(&sound_search_item, SOUNDS_SEARCH_ITEM);
		soundSystem->createSound(&sound_toilet, SOUNDS_TOILET);
		soundSystem->createSound(&sound_vent_screw, SOUNDS_VENT_SCREW);
		soundSystem->createSound(&sound_window, SOUNDS_WINDOW);
		soundSystem->createSound(&sound_yay, SOUNDS_YAY);
		soundSystem->createSound(&sound_jail_unlock, SOUNDS_JAIL_UNLOCK);
	}

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
		std::cout << "data received on client: (censored because it's " << len << " bytes)" << std::endl;
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
	gameData->decodeGameData(network_data);

	// empties the buffer after parsing
	memset(network_data, 0, sizeof(network_data)); 

	//update sound 
	
	if (myID != -1 && !soundSystem->shouldIgnoreSound()) {
		Player * player = gameData->getPlayer(myID);
		ModelType model = gameData->getPlayer(myID)->getModelType();
		WinType wt = gameData->getWT();
		Location loc = gameData->getPlayer(myID)->getLocation();

		if (player->isChef()) 
		{
			if (player->getAction() == Action::SWING_NET) {
				soundSystem->playSound(sound_net);
			}

			if (wt == WinType::CHEF_WIN) {
				soundSystem->playSound(sound_yay);
			}
		}
		else {
			//opening box
			if (player->getAction() == Action::NONE) {
				soundSystem->releaseSound(sound_search_item);
				//soundSystem->releaseSound(sound_door_unlock);
				//soundSystem->releaseSound(sound_toilet);
				//soundSystem->releaseSound(sound_vent_screw);
				//soundSystem->releaseSound(sound_jail_unlock);
			}
			else if (player->getAction() == Action::OPEN_BOX) {
				soundSystem->playSound(sound_search_item);
			}
			else if (player->getAction() == Action::CONSTRUCT_GATE) {
				int gateNum = gameData->getGateTile(loc)->getGateNum();
				if (gateNum == 1) { //door
					soundSystem->playSound(sound_door_unlock);
				}
				else if (gateNum == 2) { //bathroom
					soundSystem->playSound(sound_toilet);
				}
				else if (gateNum == 3){ //vent
					soundSystem->playSound(sound_vent_screw);
				}
			}
			else if (player->getAction() == Action::UNLOCK_JAIL) {
				soundSystem->playSound(sound_jail_unlock);
			}

			if (wt != WinType::NONE) {
				if (wt == WinType::DOOR) {
					soundSystem->playSound(sound_door);

				}
				else if (wt == WinType::TOILET) {
					soundSystem->playSound(sound_window);
				} 
				else if (wt == WinType::VENT) {
					soundSystem->playSound(sound_vent_screw);
				}
			}
		}
	}
}

GameData * ClientGame::getGameData()
{
	return gameData;
}

int ClientGame::getMyID()
{
	return myID;
}


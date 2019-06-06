#include "ClientGame.h"
#include "../graphics/GraphicsClient/SoundSystem.h"

#include <fstream>

// Comment this out to print all messages to stdout, even messages larger than 128 chars
#define CENSOR_LARGE_MSG 1024

// Paths for sounds
#define SOUNDS_PATH			"../../sounds/"
#define SOUNDS_EXIT_DOOR	(SOUNDS_PATH "door_yay_combo.mp3")
#define SOUNDS_DOOR_UNLOCK	(SOUNDS_PATH "frontexit_unlock_short.mp3")
#define SOUNDS_FOUND_ITEM	(SOUNDS_PATH "found_item_short.mp3")
#define SOUNDS_NET			(SOUNDS_PATH "pop.mp3")	// FIXME placeholder for swoosh
#define SOUNDS_RACCOON_UP	(SOUNDS_PATH "RaccoonDownUp.mp3")
#define SOUNDS_RACCOON_DOWN	(SOUNDS_PATH "RaccoonUpDown.mp3")
#define SOUNDS_SEARCH_ITEM	(SOUNDS_PATH "search_item.mp3")
#define SOUNDS_TOILET		(SOUNDS_PATH "bathroom_toilet.mp3")
#define SOUNDS_VENT_SCREW	(SOUNDS_PATH "ventexit_screw.mp3")
#define SOUNDS_EXIT_VENT	(SOUNDS_PATH "vent_yay_combo.mp3")
#define SOUNDS_EXIT_WINDOW	(SOUNDS_PATH "window_yay_combo.mp3")
#define SOUNDS_YAY			(SOUNDS_PATH "Yay.mp3")
#define SOUNDS_JAIL_UNLOCK	(SOUNDS_PATH "jail_rattle.mp3")
#define SOUNDS_KEYDROP		(SOUNDS_PATH "keydrop.mp3")
#define SOUNDS_CHEF			(SOUNDS_PATH "chef_win.mp3")

// Paths for background music loops
#define BKG_MUSIC			(SOUNDS_PATH "Safety_Net.mp3") // FIXME placeholder for background music
#define SONG_LOOP			(SOUNDS_PATH "Song_Loop.mp3")
#define LOBBY_LOOP			(SOUNDS_PATH "Lobby_Loop.mp3")
static SoundSystem * soundSystem;

// sounds specific to player (NOT 3D)
static Sound * sound_exit_door;
static Sound * sound_door_unlock;
static Sound * sound_found_item;
static Sound * sound_net;
static Sound * sound_raccoon_up;
static Sound * sound_raccoon_down;
static Sound * sound_search_item;
static Sound * sound_toilet;
static Sound * sound_vent_screw;
static Sound * sound_exit_vent;
static Sound * sound_exit_window;
static Sound * sound_yay;
static Sound * sound_jail_unlock;
static Sound * sound_keydrop;
static Sound * sound_chef;

// sounds specific to other players (3D)
static Sound * sound_other_found_item;
static Sound * sound_other_search_item;
static Sound * sound_other_jail_unlock;
static Sound * sound_other_keydrop;
static Sound * sound_other_net;
static Sound * sound_other_toilet;
static Sound * sound_other_vent_screw;
static Sound * sound_other_door_unlock;

static Sound * background_music;

// VERY HACKY FIX....
std::map<int, bool> playerDoingStuff; // true if currently doing something

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
		soundSystem->createSoundEffect(&sound_exit_door, SOUNDS_EXIT_DOOR);
		soundSystem->createSoundEffect(&sound_door_unlock, SOUNDS_DOOR_UNLOCK);
		soundSystem->createSoundEffect(&sound_found_item, SOUNDS_FOUND_ITEM);
		soundSystem->createSoundEffect(&sound_net, SOUNDS_NET);
		soundSystem->createSoundEffect(&sound_raccoon_up, SOUNDS_RACCOON_UP);
		soundSystem->createSoundEffect(&sound_raccoon_down, SOUNDS_RACCOON_DOWN);
		soundSystem->createSoundEffect(&sound_search_item, SOUNDS_SEARCH_ITEM);
		soundSystem->createSoundEffect(&sound_toilet, SOUNDS_TOILET);
		soundSystem->createSoundEffect(&sound_vent_screw, SOUNDS_VENT_SCREW);
		soundSystem->createSoundEffect(&sound_exit_vent, SOUNDS_EXIT_VENT);
		soundSystem->createSoundEffect(&sound_exit_window, SOUNDS_EXIT_WINDOW);
		soundSystem->createSoundEffect(&sound_yay, SOUNDS_YAY);
		soundSystem->createSoundEffect(&sound_jail_unlock, SOUNDS_JAIL_UNLOCK);
		soundSystem->createSoundEffect(&sound_keydrop, SOUNDS_KEYDROP);
		soundSystem->createSoundEffect(&sound_chef, SOUNDS_CHEF);

		soundSystem->createOtherPlayersSounds(&sound_other_found_item, SOUNDS_FOUND_ITEM);
		soundSystem->createOtherPlayersSounds(&sound_other_jail_unlock, SOUNDS_JAIL_UNLOCK);
		soundSystem->createOtherPlayersSounds(&sound_other_keydrop, SOUNDS_KEYDROP);
		soundSystem->createOtherPlayersSounds(&sound_other_search_item, SOUNDS_SEARCH_ITEM);
		soundSystem->createOtherPlayersSounds(&sound_other_net, SOUNDS_NET);
		soundSystem->createOtherPlayersSounds(&sound_other_toilet, SOUNDS_TOILET);
		soundSystem->createOtherPlayersSounds(&sound_other_vent_screw, SOUNDS_VENT_SCREW);
		soundSystem->createOtherPlayersSounds(&sound_other_door_unlock, SOUNDS_DOOR_UNLOCK);

		soundSystem->createBackgroundMusic(&background_music, SONG_LOOP);
		soundSystem->playBackgroundMusic(background_music, true); // FIXME: uncomment
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

	if (myID == NOT_INITIALIZED) {
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
		
	if (myID == NOT_INITIALIZED)
	{
		// initialize myID with the value sent from server
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

	// update sounds
	Player* player = gameData->getPlayer(myID);
	if (myID != NOT_INITIALIZED && player != nullptr && !soundSystem->shouldIgnoreSound()) {
		ModelType model = player->getModelType();
		WinType wt = gameData->getWT();
		Location loc = player->getLocation();
		int pNum = player->getPlayerNum();
		std::map<int, bool>::iterator miniIt;

		soundSystem->setListenerLocation(loc.getX(), loc.getY(), loc.getZ());
		miniIt = playerDoingStuff.find(pNum);
		if (miniIt == playerDoingStuff.end()) {
			playerDoingStuff.insert(std::pair<int, bool>(pNum, false));
		}

		// sounds that originate from THIS player
		if (player->isChef()) 
		{
			if (player->getAction() == Action::SWING_NET && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_net);
				playerDoingStuff[pNum] = true;
			}
			else if (player->getAction() == Action::NONE) {
				soundSystem->pauseSoundEffect();
				playerDoingStuff[pNum] = false;
			}

			if (wt == WinType::CHEF_WIN && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_chef);
				playerDoingStuff[pNum] = true;
			}
		}
		else {
			if (player->getAction() == Action::NONE) {
				soundSystem->pauseSoundEffect();
				// soundSystem->pauseSoundQueue();
				playerDoingStuff[pNum] = false;
			}
			else if (player->getAction() == Action::OPEN_BOX && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_search_item);
				playerDoingStuff[pNum] = true;
				// uncomment the below if you want to test using the sound queue
				// soundSystem->pushSoundQueue(sound_yay);
				// soundSystem->pushSoundQueue(sound_chef);
				// soundSystem->playSoundsInQueue();

				// for testing
				// soundSystem->playOtherPlayersSounds(sound_other_keydrop, 1, 0, 0, 0, true);
			}
			else if (player->getAction() == Action::CONSTRUCT_GATE && playerDoingStuff.at(pNum) == false) {
				GateTile* gateTile = gameData->getGateTile(loc);
				int gateNum = gateTile ? gateTile->getGateNum() : 0;
				if (gateNum == 1) { //door
					soundSystem->playSoundEffect(sound_door_unlock);
				}
				else if (gateNum == 2) { //bathroom
					soundSystem->playSoundEffect(sound_toilet);
				}
				else if (gateNum == 3){ //vent
					soundSystem->playSoundEffect(sound_vent_screw);
				}
				playerDoingStuff[pNum] = true;
			}
			else if (player->getAction() == Action::UNLOCK_JAIL && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_jail_unlock);
				playerDoingStuff[pNum] = true;
			}
			else if (player->getAction() == Action::KEY_DROP && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_keydrop);
				playerDoingStuff[pNum] = true;
			}

			if (wt != WinType::NONE  && playerDoingStuff.at(pNum) == false) {
				if (wt == WinType::DOOR) {
					soundSystem->playSoundEffect(sound_exit_door);
					playerDoingStuff[pNum] = true;
				}
				else if (wt == WinType::TOILET) {
					soundSystem->playSoundEffect(sound_exit_window);
					playerDoingStuff[pNum] = true;

				} 
				else if (wt == WinType::VENT) {
					soundSystem->playSoundEffect(sound_exit_vent);
					playerDoingStuff[pNum] = true;
				}
				else if (wt == WinType::CHEF_WIN) {
					soundSystem->playSoundEffect(sound_chef);
					playerDoingStuff[pNum] = true;
				}
			}

		}

		// sounds that originate from other players
		std::map < int, Player *> allPlayers = gameData->getAllPlayers();
		std::map < int, Player * >::iterator it;
		std::map < int, bool>::iterator miniIt2;
		Player * curPlayer;
		Location curPlayerLoc;
		int curPlayerNum;
		float locX = 0.0;
		float locY = 0.0;
		float locZ = 0.0;

		for (it = allPlayers.begin(); it != allPlayers.end(); it++) {
			if (it->first == myID) {
				continue;
			}

			curPlayer = it->second;
			curPlayerLoc = curPlayer->getLocation();
			curPlayerNum = it->second->getPlayerNum();

			miniIt2 = playerDoingStuff.find(curPlayerNum);
			if (miniIt2 == playerDoingStuff.end()) {
				playerDoingStuff.insert(std::pair<int, bool>(curPlayerNum, false));
			}

			locX = curPlayerLoc.getX();
			locY = curPlayerLoc.getY();
			locZ = curPlayerLoc.getZ();

			if (curPlayer->isChef())
			{
				if (curPlayer->getAction() == Action::NONE) {
					playerDoingStuff.at(curPlayerNum) = false;
					soundSystem->pauseOtherPlayersSounds(curPlayerNum);
				}
				else if (curPlayer->getAction() == Action::SWING_NET && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playOtherPlayersSounds(sound_other_net, curPlayerNum, locX, locY, locZ);
					playerDoingStuff.at(curPlayerNum) = true;
				}
			}
			else {
				if (curPlayer->getAction() == Action::NONE) {
					playerDoingStuff.at(curPlayerNum) = false;
					soundSystem->pauseOtherPlayersSounds(curPlayerNum);
				}
				else if (curPlayer->getAction() == Action::OPEN_BOX && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playOtherPlayersSounds(sound_other_search_item, curPlayerNum, locX, locY, locZ);
					playerDoingStuff.at(curPlayerNum) = true;
				}
				else if (curPlayer->getAction() == Action::CONSTRUCT_GATE && playerDoingStuff.at(curPlayerNum) == false) {
					int gateNum = gameData->getGateTile(curPlayerLoc)->getGateNum();
					if (gateNum == 1) { //door
						soundSystem->playOtherPlayersSounds(sound_other_door_unlock, curPlayerNum, locX, locY, locZ);
					}
					else if (gateNum == 2) { //bathroom
						soundSystem->playOtherPlayersSounds(sound_other_toilet, curPlayerNum, locX, locY, locZ);
					}
					else if (gateNum == 3) { //vent
						soundSystem->playOtherPlayersSounds(sound_other_vent_screw, curPlayerNum, locX, locY, locZ);
					}
					playerDoingStuff.at(curPlayerNum) = true;
				}
				else if (player->getAction() == Action::UNLOCK_JAIL && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playOtherPlayersSounds(sound_other_jail_unlock, curPlayerNum, locX, locY, locZ);
					playerDoingStuff.at(curPlayerNum) = true;
				}
				else if (player->getAction() == Action::KEY_DROP) {
					soundSystem->playOtherPlayersSounds(sound_other_keydrop, curPlayerNum, locX, locY, locZ);
				}
			}

		}

		soundSystem->update();
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


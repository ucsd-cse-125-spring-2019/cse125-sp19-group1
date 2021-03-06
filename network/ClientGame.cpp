#include "ClientGame.h"
#include "../graphics/GraphicsClient/SoundSystem.h"

#include <fstream>

// Comment this out to print all messages to stdout, even messages larger than 128 chars
#define CENSOR_LARGE_MSG 1024
//#define PRINT_DEBUG_STATEMENTS

// Paths for sounds
#define SOUNDS_PATH			"../../sounds/"
#define SOUNDS_EXIT_DOOR	(SOUNDS_PATH "door_yay_combo.mp3")
#define SOUNDS_DOOR_UNLOCK	(SOUNDS_PATH "frontexit_unlock_short.mp3")
#define SOUNDS_FOUND_ITEM	(SOUNDS_PATH "found_item_short.mp3")
#define SOUNDS_NET			(SOUNDS_PATH "NetSwoosh.mp3")
#define SOUNDS_RACCOON_UP	(SOUNDS_PATH "RaccoonDownUp.mp3")
#define SOUNDS_RACCOON_DOWN	(SOUNDS_PATH "RaccoonUpDown.mp3")
#define SOUNDS_CAT			(SOUNDS_PATH "catMeow.mp3")
#define SOUNDS_DOG			(SOUNDS_PATH "dogWhine.wav")
#define SOUNDS_SEARCH_ITEM	(SOUNDS_PATH "search_item.mp3")
#define SOUNDS_TOILET		(SOUNDS_PATH "bathroom_toilet.mp3")
#define SOUNDS_VENT_SCREW	(SOUNDS_PATH "ventexit_screw.mp3")
#define SOUNDS_EXIT_VENT	(SOUNDS_PATH "vent_yay_combo.mp3")
#define SOUNDS_EXIT_WINDOW	(SOUNDS_PATH "window_yay_combo.mp3")
#define SOUNDS_YAY			(SOUNDS_PATH "Yay.mp3")
#define SOUNDS_JAIL_UNLOCK	(SOUNDS_PATH "jail_rattle.mp3")
#define SOUNDS_KEYDROP		(SOUNDS_PATH "keydrop.mp3")
#define SOUNDS_CHEF			(SOUNDS_PATH "chef_win.mp3")
#define SOUNDS_SPLAT		(SOUNDS_PATH "splat.mp3")
#define SOUNDS_JAIL_ESCAPE	(SOUNDS_PATH "latching_opening.mp3")

// Paths for background music loops
// #define BKG_MUSIC			(SOUNDS_PATH "Safety_Net.mp3") // FIXME placeholder for background music
// #define SONG_LOOP			(SOUNDS_PATH "Song_Loop.mp3") //placeholder
#define LOBBY_LOOP			(SOUNDS_PATH "LobbyLoop.wav")
// #define INSTRUCTIONS_A	    (SOUNDS_PATH "Instructions1.wav")
// #define INSTRUCTIONS_B      (SOUNDS_PATH "Instructions2.wav")
#define LOOP_A				(SOUNDS_PATH "gameLoop.wav")
// #define LOOP_B				(SOUNDS_PATH "LoopB.wav")
// #define TRANSITION_C		(SOUNDS_PATH "TransitionC.wav")
// #define ENDING_LOOP			(SOUNDS_PATH "EndingLoop.wav")

static SoundSystem * soundSystem = nullptr;


// sounds specific to player (NOT 3D)
static Sound * sound_exit_door;
static Sound * sound_door_unlock;
static Sound * sound_found_item;
static Sound * sound_net;
static Sound * sound_raccoon_up;
static Sound * sound_raccoon_down;
static Sound * sound_cat;
static Sound * sound_dog;
static Sound * sound_search_item;
static Sound * sound_toilet;
static Sound * sound_vent_screw;
static Sound * sound_exit_vent;
static Sound * sound_exit_window;
static Sound * sound_yay;
static Sound * sound_jail_unlock;
static Sound * sound_keydrop;
static Sound * sound_chef;
static Sound * sound_splat;
static Sound * sound_jail_escape;

// sounds specific to other players (3D)
static Sound * sound_other_found_item;
static Sound * sound_other_search_item;
static Sound * sound_other_jail_unlock;
static Sound * sound_other_net;
static Sound * sound_other_toilet;
static Sound * sound_other_vent_screw;
static Sound * sound_other_door_unlock;

/*
static Sound * lobby_loop;
static Sound * instructions_one;
static Sound * instructions_two;
static Sound * game_loop_one;
static Sound * game_loop_two;
static Sound * game_transition_one;
static Sound * game_loop_three;
*/

// VERY HACKY FIXES....
std::map<int, bool> playerDoingStuff; // true if currently doing something
std::map<int, bool> playerIsCaught;
GameState prevState;

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
		soundSystem->createSoundEffect(&sound_cat, SOUNDS_CAT);
		soundSystem->createSoundEffect(&sound_dog, SOUNDS_DOG);
		soundSystem->createSoundEffect(&sound_search_item, SOUNDS_SEARCH_ITEM);
		soundSystem->createSoundEffect(&sound_toilet, SOUNDS_TOILET);
		soundSystem->createSoundEffect(&sound_vent_screw, SOUNDS_VENT_SCREW);
		soundSystem->createSoundEffect(&sound_exit_vent, SOUNDS_EXIT_VENT);
		soundSystem->createSoundEffect(&sound_exit_window, SOUNDS_EXIT_WINDOW);
		soundSystem->createSoundEffect(&sound_yay, SOUNDS_YAY);
		soundSystem->createSoundEffect(&sound_jail_unlock, SOUNDS_JAIL_UNLOCK);
		soundSystem->createSoundEffect(&sound_keydrop, SOUNDS_KEYDROP);
		soundSystem->createSoundEffect(&sound_chef, SOUNDS_CHEF);
		soundSystem->createSoundEffect(&sound_jail_escape, SOUNDS_JAIL_ESCAPE);

		soundSystem->createOtherPlayersSounds(&sound_splat, SOUNDS_SPLAT);
		soundSystem->createOtherPlayersSounds(&sound_other_found_item, SOUNDS_FOUND_ITEM);
		soundSystem->createOtherPlayersSounds(&sound_other_jail_unlock, SOUNDS_JAIL_UNLOCK);
		soundSystem->createOtherPlayersSounds(&sound_other_search_item, SOUNDS_SEARCH_ITEM);
		soundSystem->createOtherPlayersSounds(&sound_other_net, SOUNDS_NET);
		soundSystem->createOtherPlayersSounds(&sound_other_toilet, SOUNDS_TOILET);
		soundSystem->createOtherPlayersSounds(&sound_other_vent_screw, SOUNDS_VENT_SCREW);
		soundSystem->createOtherPlayersSounds(&sound_other_door_unlock, SOUNDS_DOOR_UNLOCK);

		/*
		soundSystem->createBackgroundMusic(&lobby_loop, LOBBY_LOOP); //loop that plays during the lobby, play instructions_A after the loop ends 
		soundSystem->createBackgroundMusic(&instructions_one, INSTRUCTIONS_A); //One-time song, stop after plays once, don't start InstructionsB until slide 3 comes in
		soundSystem->createBackgroundMusic(&instructions_two, INSTRUCTIONS_B); //One-time song, play immediately once slide 3 comes in, transition to Loop A
		soundSystem->createBackgroundMusic(&game_loop_one, LOOP_A); //Loop, transitions after song ends to loop B
		soundSystem->createBackgroundMusic(&game_loop_two, LOOP_B); //Loop, transitions after song ends to Transition C
		soundSystem->createBackgroundMusic(&game_transition_one, TRANSITION_C); // One-time, transitions after song ends to Ending_LOOP
		soundSystem->createBackgroundMusic(&game_loop_three, ENDING_LOOP); //Loop until game ends
		//When the game ends, immediately cease music and start playing lobby loop.
		*/

		soundSystem->createBackgroundMusic(0, LOBBY_LOOP);
		// soundSystem->createBackgroundMusic(INSTRUCTIONS_A);
		// soundSystem->createBackgroundMusic(INSTRUCTIONS_B);
		soundSystem->createBackgroundMusic(1, LOOP_A);
		// soundSystem->createBackgroundMusic(LOOP_B);
		// soundSystem->createBackgroundMusic(TRANSITION_C);
		//soundSystem->createBackgroundMusic(ENDING_LOOP);

		soundSystem->startBackgroundMusic();
		prevState = GameState::IN_LOBBY;

		// soundSystem->playBackgroundMusic(lobby_loop, true); // FIXME: uncomment
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
ClientGame::~ClientGame()
{
	delete gameData;
	delete network;

	if (soundSystem) {

		if (!soundSystem->shouldIgnoreSound()) {
			soundSystem->releaseSound(sound_exit_door);
			soundSystem->releaseSound(sound_door_unlock);
			soundSystem->releaseSound(sound_found_item);
			soundSystem->releaseSound(sound_net);
			soundSystem->releaseSound(sound_raccoon_down);
			soundSystem->releaseSound(sound_raccoon_up);
			soundSystem->releaseSound(sound_cat);
			soundSystem->releaseSound(sound_dog);
			soundSystem->releaseSound(sound_search_item);
			soundSystem->releaseSound(sound_toilet);
			soundSystem->releaseSound(sound_vent_screw);
			soundSystem->releaseSound(sound_exit_window);
			soundSystem->releaseSound(sound_yay);
			soundSystem->releaseSound(sound_jail_unlock);
			soundSystem->releaseSound(sound_keydrop);
			soundSystem->releaseSound(sound_chef);
			soundSystem->releaseSound(sound_splat);
			soundSystem->releaseSound(sound_jail_escape);

			soundSystem->releaseSound(sound_other_found_item);
			soundSystem->releaseSound(sound_other_jail_unlock);
			soundSystem->releaseSound(sound_other_search_item);
			soundSystem->releaseSound(sound_other_net);
			soundSystem->releaseSound(sound_other_toilet);
			soundSystem->releaseSound(sound_other_vent_screw);
			soundSystem->releaseSound(sound_other_door_unlock);

			/*
			soundSystem->releaseSound(lobby_loop);
			soundSystem->releaseSound(instructions_one);
			soundSystem->releaseSound(instructions_two);
			soundSystem->releaseSound(game_loop_one);
			soundSystem->releaseSound(game_loop_two);
			soundSystem->releaseSound(game_transition_one);
			soundSystem->releaseSound(game_loop_three);
			*/
		}

		delete soundSystem;

		soundSystem = nullptr;
	}
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
#ifdef PRINT_DEBUG_STATEMENTS
#ifdef CENSOR_LARGE_MSG
	const auto len = strlen(network_data);
	if (len > CENSOR_LARGE_MSG)
		std::cout << "data received on client: (censored because it's " << len << " bytes)" << std::endl;
	else
#endif
		std::cout << "data received on client:\n" << network_data << std::endl;
#endif
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
		miniIt = playerIsCaught.find(pNum);
		if (miniIt == playerIsCaught.end()) {
			playerIsCaught.insert(std::pair<int, bool>(pNum, false));
		}

		// sounds that originate from THIS player
		if (player->isChef()) 
		{
			if (player->getAction() == Action::SWING_NET && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_net);
				playerDoingStuff[pNum] = true;

				if (player->getCaughtAnimalType() == ModelType::RACOON) {
					soundSystem->playSoundEffect(sound_raccoon_up, true);
				}
				else if (player->getCaughtAnimalType() == ModelType::CAT) {
					soundSystem->playSoundEffect(sound_cat, true);
				}
				else if (player->getCaughtAnimalType() == ModelType::DOG) {
					soundSystem->playSoundEffect(sound_dog, true);
				}
			}
			else if (player->getAction() == Action::NONE) {
				soundSystem->pauseSoundEffect();
				playerDoingStuff[pNum] = false;
			}
			else if (player->getAction() == Action::DESTROY_POWERUP && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_splat, true);
				playerDoingStuff[pNum] = true;
			}

			if (wt == WinType::CHEF_WIN && playerDoingStuff.at(pNum) == false) {
				soundSystem->playSoundEffect(sound_chef);
				playerDoingStuff[pNum] = true;
			}
		}
		else {
			if (player->getAction() == Action::NONE) {
				soundSystem->pauseSoundEffect();
				playerDoingStuff[pNum] = false;

				if (player->isCaught() == false && playerIsCaught.at(pNum)) {
					soundSystem->playSoundEffect(sound_jail_escape, true);
					playerIsCaught[pNum] = false;
				}
				if (player->isCaught()) {
					playerIsCaught[pNum] = true;
				}
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
				soundSystem->playSoundEffect(sound_keydrop, true);
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

			if (player->getBearBuff()) {
				std::cout << "I HAVE BEAR BUFF" << std::endl;
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
			miniIt2 = playerIsCaught.find(curPlayerNum);
			if (miniIt2 == playerIsCaught.end()) {
				playerIsCaught.insert(std::pair<int, bool>(curPlayerNum, false));
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

					if (curPlayer->getCaughtAnimalType() == ModelType::RACOON) {
						soundSystem->playSoundEffect(sound_raccoon_up, true);
					}
					else if (curPlayer->getCaughtAnimalType() == ModelType::CAT) {
						soundSystem->playSoundEffect(sound_cat, true);
					}
					else if (curPlayer->getCaughtAnimalType() == ModelType::DOG) {
						soundSystem->playSoundEffect(sound_dog, true);
					}
				}
				else if (curPlayer->getAction() == Action::DESTROY_POWERUP && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playOtherPlayersSounds(sound_splat, curPlayerNum, locX, locY, locZ, true);
					playerDoingStuff[curPlayerNum] = true;
				}
			}
			else {
				if (curPlayer->getAction() == Action::NONE) {
					playerDoingStuff.at(curPlayerNum) = false;
					soundSystem->pauseOtherPlayersSounds(curPlayerNum);

					if (curPlayer->isCaught() == false && playerIsCaught.at(curPlayerNum)) {
						soundSystem->playSoundEffect(sound_jail_escape, true);
						playerIsCaught[curPlayerNum] = false;
					}
					if (curPlayer->isCaught()) {
						playerIsCaught[curPlayerNum] = true;
					}
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
				else if (curPlayer->getAction() == Action::UNLOCK_JAIL && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playOtherPlayersSounds(sound_other_jail_unlock, curPlayerNum, locX, locY, locZ);
					playerDoingStuff.at(curPlayerNum) = true;
				}
				else if (curPlayer->getAction() == Action::KEY_DROP && playerDoingStuff.at(curPlayerNum) == false) {
					soundSystem->playSoundEffect(sound_keydrop, true);
					playerDoingStuff.at(curPlayerNum) = true;
				}
			}

		}

		// background music checks
		GameState currentState = gameData->getGameState();
		if (prevState == GameState::IN_LOBBY && currentState == GameState::LOADING) {
			// soundSystem->nextBackgroundLoop();
			prevState = GameState::LOADING;
		}
		else if (prevState == GameState::LOADING && currentState == GameState::IN_GAME) {
			soundSystem->nextBackgroundLoop(1);
			prevState = GameState::IN_GAME;

		}
		else if (prevState == GameState::IN_GAME && currentState == GameState::WIN_CUTSCENE) {
			soundSystem->nextBackgroundLoop(0);
			prevState = GameState::WIN_CUTSCENE;
		}
		else if (currentState == GameState::IN_LOBBY) {
			prevState = GameState::IN_LOBBY;
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


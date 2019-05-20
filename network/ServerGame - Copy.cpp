#include "ServerGame - Copy.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
 
unsigned int ServerGame::client_id; 

ServerGame::ServerGame(void)
{
	newPlayerInit = false;
    // id's to assign clients for our table
    client_id = 0;
 
    // set up the server network to listen 
    network = new ServerNetwork(); 

	gameManager = new GameManager();
}
 
void ServerGame::update() 
{
	auto start_time = std::chrono::high_resolution_clock::now();

    // get new clients
    if(network->acceptNewClient(client_id))
    {
        printf("client %d has been connected to the server\n",client_id); 
    }
	receiveFromClients();
	//sendActionPackets();

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	//std::cout << " took " << time / std::chrono::milliseconds(1) << "ms to run.\n";
}

void ServerGame::receiveFromClients()
{

	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;


	for (iter = network->sessions.begin(); iter != network->sessions.end(); )//iter++)
	{
		int playerID = iter->first;
		int data_length = network->receiveData(iter->first, network_data);
		//cout << "size:" << network->sessions.size() << std::endl;
		if (data_length <= 0)
		{
			//no data recieved
			if (network->sessions.size() == 0)
				break;

			if (data_length == 0 || (data_length == -1 && (WSAGetLastError() == CONNECTION_RESET_ERROR || WSAGetLastError() == CONNECTION_ABORT_ERROR)))
			{
				printf("Client disconnected\n");
				closesocket(iter->second);
				//gameData->removePlayer(playerID, ClientType::SERVER_SIDE);
				gameManager->removePlayer(playerID, ClientType::SERVER_SIDE);
				network->sessions.erase(iter++);
			}
			else
			{
				iter++;
			}

			continue;
		}

		gameManager->resetPlayerDirectionFlags();
		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			if (packet.packet_type != INIT_CONNECTION && packet.id == -1) {
				continue;
			}

			switch (packet.packet_type) {
			case INIT_CONNECTION:
				newPlayerInit = true;
				printf("server received init packet from client\n");
				initNewClient();
				sendInitPackets();
				break;

			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				break;
			case READY_EVENT:
				printf("server received ENTER event packet from client\n");
				if (!gameManager->gameHasStarted())
					gameManager->togglePlayerReady(playerID);
				break;
			case START_EVENT:
				printf("server received START event packet from client\n");
				if (!gameStarted && allPlayersReady)
				{
					//initCharacters = true;
					gameData->startCountdown();
				}
				break;
			case SELECT_EVENT:
				printf("server received SELECT event packet from client\n");
				break;
			case SELECT0_EVENT:
				printf("server received SELECT0 event packet from client\n");
				//gameData->getPlayer(iter->first)->setModelType(ModelType::DEFAULT);

				break;
			case SELECT1_EVENT:
				printf("server received SELECT1 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::CHEF);
				break;
			case SELECT2_EVENT:
				printf("server received SELECT2 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::RACOON);
				break;
			case SELECT3_EVENT:
				printf("server received SELECT3 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::CAT);
				break;
			case SELECT4_EVENT:
				printf("server received SELECT4 event packet from client\n");
				gameData->getPlayer(iter->first)->setModelType(ModelType::DOG);
				break;
			case FORWARD_EVENT:
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					gameManager->updateMovement(1, playerID);
				}
				break;

			case BACKWARD_EVENT:
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					gameManager->updateMovement(2, playerID);
				}
				break;

			case LEFT_EVENT:
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					gameManager->updateMovement(3, playerID);
				}
				break;

			case RIGHT_EVENT:
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					gameManager->updateMovement(4, playerID);
				}
				break;

			case INTERACT_EVENT:
			{
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					gameManager->interactEvent(playerID);
				}
				break;
			}

			case RELEASE_EVENT:
			{
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->getInteracting() ||
							player->getIsCaught() ||
							player->getHidden()) {
							break;
						}

						Location loc = player->getLocation();

						if (player->getInteracting()) {
							double seconds = player->getInteractingTime(0);
							if (seconds > gameData->getBoxTime()) {
								std::cout << "UPDATED BOX UNLOCKED KEY" << std::endl;
								gameData->getAtlas()->updateBoxLayout(loc);
								player->setInteracting(false);
							}
						}

						if (player->getInteracting() &&
							!player->isChef()) {
							std::cout << "RELEASED SPACE" << std::endl;
							player->setInteracting(false);
						}


						if (player->getOpeningGate() &&
							!player->isChef() && gameData->getAtlas()->hasGate(loc))
						{
							GateTile * gateTile = (GateTile *)(gameData->getAtlas()->getTileAt(loc));
							if (!gateTile->isOpen())
							{
								std::cout << "RELEASED SPACE" << std::endl;
								player->setOpeningGate(false);

								//update progress of gate 
								double seconds = player->getInteractingTime(0);
								gateTile->constructGate(seconds);
							}
						}
					}
				}
			
				break;
			}
			case DROP_EVENT:
			{
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->getInteracting() ||
							player->getIsCaught() ||
							player->getHidden()) {
							break;
						}

						std::cout << "facing:" << static_cast<int>(player->getFacingDirection()) << std::endl;
						Location loc = player->getLocation();

						// PLayer cannot drop item if there is an item already on the current tile
						if (!(gameData->getAtlas()->tileHasItem(loc)))
						{
							ItemName itemName = player->getInventory();
							gameData->getAtlas()->updateTileItem(loc, itemName);
							player->setInventory(ItemName::EMPTY);

							gameData->getAtlas()->updateDroppedItem(itemName, loc);

						}
					}
				}
				break;
			}
			case HIDE_EVENT:
			{
				if (gameManager->gameHasStarted() && !gameManager->gameIsOver())
				{
					if (Player * player = gameData->getPlayer(iter->first))
					{
						if (player->getInteracting() ||
							player->getIsCaught()) {
							break;
						}

						Location loc = player->getLocation();
						if (!(gameData->getAtlas()->hasHide(loc))) { return; }

						HideTile * hideTile = (HideTile *)(gameData->getAtlas()->getTileAt(loc));

						if (player->isChef()) {
							if (!hideTile->checkHideTileEmpty())
							{
								int animal = hideTile->getAnimalHiding();
								gameData->getPlayer(animal)->setIsCaught(true);
								player->setCaughtAnimalId(animal);
								player->setCaughtAnimal(true);
								player->setHidden(false);
								hideTile->setAnimalHiding(-1);
							}
						}
						else {
							if (player->getHidden())
							{
								player->setHidden(false);
								hideTile->setAnimalHiding(-1);
							}
							else if (hideTile->checkHideTileEmpty())
							{
								//set hideTile full
								hideTile->setAnimalHiding(iter->first);
								//set animal to hidden
								player->setHidden(true);
							}
						}
					}
				}
				break;
			}
			case RESET_EVENT:
			{
				std::cout << "RESET_EVENT" << std::endl;
				if (gameManager->gameIsOver())
				{
					std::cout << "CALLING RESET" << std::endl;
					gameManager->resetGame();
				}
				break;
			}

			default:
				printf("error in packet types\n");
				break;
			}
		}
		
		sendActionPackets(); // sends data after processing input from one clientss
		iter++;
	}
	gameManager->update();
	//sendActionPackets(); // uncomment to always send data from server
	if (gameManager->isCountingDown())
	{
		sendActionPackets();
	}
}

void ServerGame::sendInitPackets()
{
	std::string msg_string = "init: " + std::to_string(client_id) + "\n";
	client_id++;

	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	for (int i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	network->sendToAll(msg, packet_size);
	delete[] msg;
}

void ServerGame::sendActionPackets()
{
	// gets an encoded game data string for all players and general game data
	std::string msg_string = gameManager->encodeGameData(newPlayerInit);
	newPlayerInit = false;
	int packet_size = msg_string.length();
	char * msg = new char[packet_size];

	for (int i = 0; i < packet_size; i++) {
		msg[i] = msg_string[i];
	}

	network->sendToAll(msg, packet_size);
	delete[] msg;
}

void ServerGame::initNewClient()
{
	//updating current data structure to hold onto client
	gameManager->addNewPlayer(client_id, Location(30, 0, 30), ClientType::SERVER_SIDE);
}

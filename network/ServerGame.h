#include "ServerNetwork.h"
#include "NetworkData.h"
#include "Atlas.h"
#include <string>
#include "GameData.h"

class ServerGame
{

public:

	ServerGame(void);
	~ServerGame(void);

    void update();
	void receiveFromClients();
	void sendActionPackets();
	void sendInitPackets();
	void initNewClient();
	void updateForwardEvent(int id);
	void updateBackwardEvent(int id);
	void updateLeftEvent(int id);
	void updateRightEvent(int id);
	void updateHeight(int id);
	void updateCollision(int id);
	void updatePlayerCollision(int id, int dir);
	void resetGame();
	void updateMovement(int dir, int id);

	GameData * gameData;
private:

    // IDs for the clients connecting for table in ServerNetwork 
    unsigned int client_id;

    // The ServerNetwork object 
    ServerNetwork* network;

	//Atlas* atlas;
	float sSpeed = 1.2;
	float dSpeed = sSpeed/1.414;

	// data buffer
	char network_data[MAX_PACKET_SIZE];
	
	bool newPlayerInit;
	bool gameStarted;
	bool allPlayersReady;
	bool initCharacters;

	bool moveForward, moveBackward, moveLeft, moveRight;
	bool chefWin;
	bool animalWin;
};
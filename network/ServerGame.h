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
	void updateCollision(int id);
	void updatePlayerCollision(int id, int dir);

	GameData * gameData;
private:

    // IDs for the clients connecting for table in ServerNetwork 
    static unsigned int client_id;

    // The ServerNetwork object 
    ServerNetwork* network;

	Atlas* atlas;

	// data buffer
	char network_data[MAX_PACKET_SIZE];
	
	bool newPlayerInit;
};
#include "ServerNetwork.h"
#include "NetworkData.h"
#include "Walls.h"
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
	void updateForwardEvent(std::string id);
	void updateBackwardEvent(std::string id);
	void updateLeftEvent(std::string id);
	void updateRightEvent(std::string id);
	void updateCollision(std::string id);
	void updatePlayerCollision(std::string id, int dir);

	GameData * gameData;
private:

    // IDs for the clients connecting for table in ServerNetwork 
    static unsigned int client_id;

    // The ServerNetwork object 
    ServerNetwork* network;

	// The Walls object
	Walls* walls;

	// data buffer
	char network_data[MAX_PACKET_SIZE];
	
};
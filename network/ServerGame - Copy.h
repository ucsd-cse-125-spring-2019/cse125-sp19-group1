//#include "ServerNetwork.h"
//#include "NetworkData.h"
////#include "Atlas.h"
//#include <string>
//#include "GameData.h"
//#include "GameManager.h"
//
//class ServerGame
//{
//
//public:
//
//	ServerGame(void);
//	~ServerGame(void);
//
//    void update();
//	void receiveFromClients();
//	void sendActionPackets();
//	void sendInitPackets();
//	void initNewClient();
//
//
//private:
//
//    // IDs for the clients connecting for table in ServerNetwork 
//	unsigned int client_id;
//
//    // The ServerNetwork object 
//    ServerNetwork* network;
//
//	// data buffer
//	char network_data[MAX_PACKET_SIZE];
//	
//	bool newPlayerInit;
//	//bool initCharacters;
//	GameManager * gameManager;
//};
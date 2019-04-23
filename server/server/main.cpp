// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"
// used for multi-threading
#include <process.h>
#include <ctime>

void serverLoop(void *);
void clientLoop(void);
 
ServerGame * server;
ClientGame * client;
int elapsedTime = 0;

int main()
{
 
    // initialize the server
    server = new ServerGame();

    // create thread with arbitrary argument for the run function
    //_beginthread( serverLoop, 0, (void*)12);
 
    // initialize the client 
    //client = new ClientGame();
    
    //clientLoop();
	serverLoop((void*)12);
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
		if (clock() - elapsedTime > 1000.0 / 30)
		{
			elapsedTime = clock();
			server->update();
		}
    }
}
 
void clientLoop()
{
	bool xd = true;
	while (true)
	{
		//do game stuff
		//client->sendForwardPackets();
		client->update();
		if (true) 
		{
			//client->sendMovementPackets(FORWARD_EVENT);
			//client->sendMovementPackets(BACKWARD_EVENT);
			//client->sendMovementPackets(LEFT_EVENT);
			//client->sendMovementPackets(RIGHT_EVENT);
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			xd = false;
		}

	}
}
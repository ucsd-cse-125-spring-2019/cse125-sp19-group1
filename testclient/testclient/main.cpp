// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"
// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void);
 
ServerGame * server;
ClientGame * client;

int main()
{
 
    // initialize the server
    //server = new ServerGame();

    // create thread with arbitrary argument for the run function
    //_beginthread( serverLoop, 0, (void*)12);
 
    // initialize the client 
    client = new ClientGame();
    
    clientLoop();
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
        server->update();
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
		if (xd) 
		{
			client->sendForwardPackets();
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			//client->sendForwardPackets();
			xd = false;
		}

	}
}
// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "../network/ServerGame.h"
#include "../network/ClientGame.h"
// used for multi-threading
#include <process.h>
#include <ctime>

void serverLoop(void *);

ServerGame * server;
int elapsedTime = 0;
int elapsedTime2 = 0;

int main()
{
 
    // initialize the server
    server = new ServerGame();

	serverLoop((void*)12);
}

void readDataLoop(void * arg)
{
	while (true)
	{
		if (clock() - elapsedTime2 > 1000.0 / 30)
		{
			elapsedTime2 = clock();
			server->update();
		}
	}
}
void serverLoop(void * arg) 
{ 
    while(true) 
    {
		if (clock() - elapsedTime > 1000.0 / 30)
		{
			elapsedTime = clock();
			server->update();
			//server->sendActionPackets();
		}
    }
}
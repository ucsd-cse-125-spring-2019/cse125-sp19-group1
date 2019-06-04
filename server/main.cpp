// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "../network/ServerGame.h"
#include "../network/ClientGame.h"
// used for multi-threading
#include <process.h>
#include <ctime>
#include <chrono>
#define FPS 60

void serverLoop(void *);

ServerGame * server;
std::chrono::time_point<std::chrono::system_clock> timeStamp = std::chrono::system_clock::now();


int main()
{
 
    // initialize the server
    server = new ServerGame();

	serverLoop((void*)12);
	auto now = std::chrono::system_clock::now();
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - timeStamp;
		if (elapsed_seconds.count() > 1000.0 / FPS)
		{
			timeStamp = std::chrono::system_clock::now();
			server->update();
		}
    }
}
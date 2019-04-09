#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"

class ClientGame
{

public:
	ClientGame();
	~ClientGame(void);

	ClientNetwork* network;
};
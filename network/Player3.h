#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include "Item.h"
#include "Location.h"

#define NUM_PLAYER_MODEL_TYPES (4)
enum class ModelType { DEFAULT = -1, CHEF = 0, RACOON, CAT, DOG };

enum class Action { OPEN_BOX, CONSTRUCT_GATE, UNLOCK_JAIL, SWING_NET };

class Player
{
public:

protected:
	// General
	int			playerID;
	ModelType	modelType;
	Location	location;

	bool		interacting;
	std::chrono::time_point<std::chrono::system_clock> actionStartTime;

	// Chef Only
	bool		caughtAnimal = false;
	int			radius = 10;
	int			caughtAnimalId;

	// Animal Only
	ItemName	inventory;
	bool		openingJail;
	bool		openingGate;
	bool		isCaught = false;
	std::chrono::time_point<std::chrono::system_clock> startJail;
	std::chrono::time_point<std::chrono::system_clock> startGate;
};
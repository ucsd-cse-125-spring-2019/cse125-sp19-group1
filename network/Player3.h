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

	Player();
	Player(int anID, ModelType type, Location aLoc = Location{0, 0, 0});

	/* General */
	bool		isReady() const;
	ModelType	getModelType() const;
	Location	getLocation() const;
	bool		getInteracting() const;
	double		getInteractingTime(int opt);

	void setModelType(ModelType type);
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
	void toggleReady();
	void setInteracting(bool interact);

	virtual std::string encodePlayerData(bool newPlayerInit);
	void decodePlayerData(std::string key, std::string value);

	// Encode functions
	virtual void addEncodeFunctions();
	std::string encodeModelType();
	std::string encodeLocation();

	// Decode functions
	void addDecodeFunctions();
	void decodeModelType(std::string value);
	void decodeLocation(std::string value);
	
	/* Chef --------------------------------------------------------------------------------*/
	int			getCaughtAnimalId() const;
	bool		getCaughtAnimal() const;

	
	bool inRange(Location & myLoc, Location & theirLoc);
	void setCaughtAnimalId(int id);
	void setCaughtAnimal(bool caught);

	
	
	std::string encodePlayerData(bool newPlayerInit);
	void decodePlayerData(std::string key, std::string value);

	// Encode functions
	void addEncodeFunctions();

	// Decode functions
	void addDecodeFunctions();

	/* Animal ------------------------------------------------------------------------------*/

	ItemName	getInventory() const;


	void setIsCaught(bool caught);
	void setInventory(ItemName anItem);
	bool		getIsCaught() const;


	
	std::string encodePlayerData(bool newPlayerInit);
	void decodePlayerData(std::string key, std::string value);

	// Encode functions
	void addEncodeFunctions();
	std::string encodeInventory();

	// Decode functions
	void addDecodeFunctions();
	void decodeInventory(std::string value);

	
protected:
	// General
	int			playerID;
	ModelType	modelType;
	Location	location;

	bool		ready = true;
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
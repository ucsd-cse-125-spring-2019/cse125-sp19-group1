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

#include "GameConfigs.h"

#define NUM_PLAYER_MODEL_TYPES (4)
enum class ModelType { DEFAULT = -1, CHEF = 0, RACOON, CAT, DOG};
enum class Direction {
	NORTH = 5,
	SOUTH = -5,
	EAST = -1,
	WEST = 1,
	NORTHEAST = NORTH + EAST,
	NORTHWEST = NORTH + WEST,
	SOUTHEAST = SOUTH + EAST,
	SOUTHWEST = SOUTH + WEST
};

enum class Action { NONE, OPEN_BOX, CONSTRUCT_GATE, UNLOCK_JAIL, SWING_NET };

class Player
{
public:
	
	// Constructors
	Player();
	Player(int anID, Location aLoc);

	// Getters
	ItemModelType	getInventory() const;
	bool		isInteracting() const;
	Action		getAction() const;
	//bool		getOpenJail() const;
	//bool		getOpeningGate() const;
	Location	getLocation() const;
	ModelType	getModelType() const;
	bool		isChef() const;
	bool		getCaughtAnimal() const;
	bool		getIsCaught() const;
	int			getCaughtAnimalId() const;

	bool		isReady() const;

	Direction getFacingDirection() const;

	// Setters
	void setInventory(ItemModelType anItem);
	void setModelType(ModelType type);
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
	bool getHidden();
	void setHidden(bool hide);

	void setInteracting(bool interact);
	//void setOpenJail(bool interact);
	//void setOpeningGate(bool status);
	void setCaughtAnimal(bool caught);
	void setIsCaught(bool caught);
	void setCaughtAnimalId(int id);
	void setActionStartTime();
	void setUnlockJailStartTime();
	void setAction(Action anAction);

	void toggleReady();
	
	void setFacingDirection(Direction dir);

	bool inRange(Location & myLoc, Location & theirLoc);

	
	double getInteractingTime(int opt);

	std::string encodePlayerData(bool newPlayerInit);
//	std::string encodePlayerData() const;
	void decodePlayerData(std::string key, std::string value);

	// Decode functions
	void addDecodeFunctions();
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeModelType(std::string value);
	void decodeHidden(std::string value);
	void decodeInteractAction(std::string value);
	void decodeVisionRadius(std::string value);

	// Encode functions
	void addEncodeFunctions();
	std::string encodeLocation();
	std::string encodeInventory();
	std::string encodeModelType();
	std::string encodeHidden();
	std::string encodeInteractAction();
	std::string encodeVisionRadius();

protected:
	Action		action;
	Location	location;
	int			playerID;
	ItemModelType	inventory;
	ModelType	modelType;
	bool		interacting;
	//bool		openingJail;
	//bool		openingGate;
	bool		isCaught = false;
	int			catchRadius = 10;
	bool		caughtAnimal = false;
	int			caughtAnimalId;
	bool		hidden = false;
	float		visionRadius;

	std::chrono::time_point<std::chrono::system_clock> actionStartTime;
	std::chrono::time_point<std::chrono::system_clock> unlockJailStartTime;

	std::map < std::string, bool> dirtyVariablesMap;
	using decodeFunctionType = void (Player::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	using encodeFunctionType = std::string(Player::*)();
	std::map<std::string, encodeFunctionType> encodingFunctions;


	bool ready = false;

	Direction facingDirection;
};
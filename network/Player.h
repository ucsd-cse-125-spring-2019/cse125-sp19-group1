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

class Player
{
public:
	
	// Constructors
	Player();
	Player(int anID);
	Player(int anID, Location aLoc);

	// Getters
	ItemModelType	getInventory() const;
	bool		getInteracting() const;
	bool		getOpenJail() const;
	bool		getOpeningGate() const;
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
	bool getInteracting();

	void setInteracting(bool interact);
	void setOpenJail(bool interact);
	void setOpeningGate(bool status);
	void setCaughtAnimal(bool caught);
	void setIsCaught(bool caught);
	void setCaughtAnimalId(int id);
	void setActionStartTime();
	void setStartJailTime();
	void setSpeedStartTime();
	void setSearchStartTime();
	void setVisionStartTime();
	void setSlowStartTime();

	double getSpeedTime();
	double getSearchTime();
	double getVisionTime();
	double getSlowTime();
	
	bool getPoweringUp() { return poweringUp; }
	bool getOpeningBox() { return openingBox; }
	void setPoweringUp(bool power) { poweringUp = power; }
	void setOpeningBox(bool interact) { openingBox = interact; }


	void toggleReady();
	
	void setFacingDirection(Direction dir);

	bool inRange(Location & myLoc, Location & theirLoc);

	double getInteractingTime(int opt);

	//chef interaction/power-up methods
	double chefSpeedMultiplier = 1.0;
	double slowedSpeed = 0.5;
	bool slowChef = false;
	bool getSlowChef() { return slowChef; }
	void toggleSlowChef() { slowChef = !slowChef; }

	bool blindChef = false;
	bool getBlindChef() { return blindChef; }
	void toggleBlindChef() { blindChef = !blindChef; }

	double getChefSpeedMultiplier() { return chefSpeedMultiplier; }
	void updateChefMultiplier(int anger);

	//player interaction/power-up methods
	bool ghost = false;
	bool getGhost() { return ghost; }
	void toggleGhost() { ghost = !ghost; }

	bool instantSearch = false;
	bool getInstantSearch() { return instantSearch; }
	void toggleInstantSearch() { instantSearch = !instantSearch; }

	std::string encodePlayerData(bool newPlayerInit);
//	std::string encodePlayerData() const;
	void decodePlayerData(std::string key, std::string value);

	// Decode functions
	void addDecodeFunctions();
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeCakeStatus(std::string value);
	void decodeModelType(std::string value);
	void decodeHidden(std::string value);

	// Encode functions
	void addEncodeFunctions();
	std::string encodeLocation();
	std::string encodeInventory();
	std::string encodeCakeStatus();
	std::string encodeModelType();
	std::string encodeHidden();

protected:
	Location	location;
	int			playerID;
	ItemModelType	inventory;
	bool		hasCake;
	ModelType	modelType;
	bool		interacting;
	bool		poweringUp;
	bool		openingJail;
	bool		openingGate;
	bool		openingBox;
	bool		caughtAnimal = false;
	bool		isCaught = false;
	int			radius = 10;
	int			caughtAnimalId;
	bool		hidden = false;

	std::chrono::time_point<std::chrono::system_clock> actionStartTime;
	std::chrono::time_point<std::chrono::system_clock> startJail;
	std::chrono::time_point<std::chrono::system_clock> startGate;
	std::chrono::time_point<std::chrono::system_clock> speedStartTime;
	std::chrono::time_point<std::chrono::system_clock> searchStartTime;
	std::chrono::time_point<std::chrono::system_clock> visionStartTime;
	std::chrono::time_point<std::chrono::system_clock> slowStartTime;

	std::map < std::string, bool> dirtyVariablesMap;
	using decodeFunctionType = void (Player::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	using encodeFunctionType = std::string(Player::*)();
	std::map<std::string, encodeFunctionType> encodingFunctions;


	bool ready = false;

	Direction facingDirection;
};
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

enum class Action { NONE, OPEN_BOX, CONSTRUCT_GATE, UNLOCK_JAIL, SWING_NET, KEY_DROP, DESTROY_POWERUP };

enum class PowerUp {NONE, FLASH, GHOST, INSTA_SEARCH, CHEF_BLIND, CHEF_SLOW};

class Player
{
public:
	
	// Constructors
	Player();
	Player(int anID, int aPlayerNum, Location aLoc);

	// Getters
	int			getPlayerNum() const;
	ItemModelType	getInventory() const;
	bool		isInteracting() const;
	Action		getAction() const;
	PowerUp		getPowerUp() const;
	Location	getLocation() const;
	ModelType	getModelType() const;
	bool		isChef() const;
	bool		isCaught() const;
	bool		hasCaughtAnimal() const;
	int			getCaughtAnimalId() const;
	ModelType	getCaughtAnimalType() const;
	bool		hasSelectedAnimal() const;
	double		getSpeedMultiplier() const;
	float		getVisionRadius() { return visionRadius; }
	Direction	getFacingDirection() const;
	bool		isDoneLoading() const;
	int			getCatchRadius() const;

	// Setters
	void setPlayerID(int id);
	void setVisionRadius(float vr);
	void toggleAnimalSelection();
	void setInventory(ItemModelType anItem);
	void setModelType(ModelType type);
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
	bool getHidden();
	void setHidden(bool hide);
	void modifySpeedMultiplier(double multiplier);
	void setInteracting(bool interact);
	void setCaughtStatus(bool caught);
	void setCaughtAnimal(bool caught);
	void setCaughtAnimalId(int id);
	void setCaughtAnimalType(ModelType type);
	void setActionStartTime();
	void setUnlockJailStartTime();
	void setAction(Action anAction);
	void setPowerUp(PowerUp aPowerUp);
	void setFacingDirection(Direction dir);
	void setDoneLoading(bool status);

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

	bool inRange(Location & myLoc, Location & theirLoc);

	double getInteractingTime(int opt);

	//chef interaction/power-up methods
	double chefSpeedMultiplier = 1.0;
	double slowedSpeed = 0.5;
	bool slowChef = false;
	bool getSlowChef() { return slowChef; }
	void setSlowChef(bool slow) { slowChef = slow; }

	bool blindChef = false;
	bool getBlindChef() { return blindChef; }
	void setBlindChef(bool blind) { blindChef = blind; }

	double getChefSpeedMultiplier() { return chefSpeedMultiplier; }
	void updateChefMultiplier(int anger);

	void resetSpeedMultiplier();
	void setDefaultSpeedMultiplier(double multiplier);
	//player interaction/power-up methods
	bool ghost = false;
	bool getGhost() { return ghost; }
	void setGhost(bool aGhost) { ghost = aGhost; }

	bool instantSearch = false;
	bool getInstantSearch() { return instantSearch; }
	void setInstantSearch(bool is) { instantSearch = is; }

	std::string encodePlayerData(bool newPlayerInit);
	void decodePlayerData(std::string key, std::string value);

	// Decode functions
	void addDecodeFunctions();
	void decodePlayerNum(std::string value);
	void decodeAnimalSelection(std::string value);
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeModelType(std::string value);
	void decodeHidden(std::string value);
	void decodeInteractAction(std::string value);
	void decodeInteractPowerUp(std::string value);
	void decodeVisionRadius(std::string value);
	void decodeCaughtStatus(std::string value);
	void decodeCaughtAnimal(std::string value);
	void decodeCaughtAnimalID(std::string value);
	void decodeCaughtAnimalType(std::string value);
	void decodeFacingDirection(std::string value);

	// Encode functions
	void addEncodeFunctions();
	std::string encodePlayerNum();
	std::string encodeAnimalSelection();
	std::string encodeLocation();
	std::string encodeInventory();
	std::string encodeModelType();
	std::string encodeHidden();
	std::string encodeInteractAction();
	std::string encodeInteractPowerUp();
	std::string encodeVisionRadius();
	std::string encodeCaughtStatus();
	std::string encodeCaughtAnimal();
	std::string encodeCaughtAnimalID();
	std::string encodeCaughtAnimalType();
	std::string encodeFacingDirection();

protected:
	Action		action;
	PowerUp		powerUp;
	Location	location;
	int			playerID;
	int			playerNum;
	bool		selectedAnimal;
	ItemModelType	inventory;
	ModelType	modelType;
	bool		interacting;
	bool		caughtStatus = false;
	int			catchRadius = 10;
	Direction facingDirection;

	bool		poweringUp;
	bool		openingJail;
	bool		openingGate;
	bool		openingBox;
	bool		doneLoading;

	bool		caughtAnimal = false;
	int			caughtAnimalID;
	ModelType	caughtAnimalType;
	bool		hidden = false;
	float		visionRadius;
	double		speedMultiplier;
	double		defaultSpeedMultiplier;

	std::chrono::time_point<std::chrono::system_clock> actionStartTime;
	std::chrono::time_point<std::chrono::system_clock> unlockJailStartTime;

	std::chrono::time_point<std::chrono::system_clock> speedStartTime;
	std::chrono::time_point<std::chrono::system_clock> searchStartTime;
	std::chrono::time_point<std::chrono::system_clock> visionStartTime;
	std::chrono::time_point<std::chrono::system_clock> slowStartTime;

	std::map < std::string, bool> dirtyVariablesMap;
	using decodeFunctionType = void (Player::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	using encodeFunctionType = std::string(Player::*)();
	std::map<std::string, encodeFunctionType> encodingFunctions;

};
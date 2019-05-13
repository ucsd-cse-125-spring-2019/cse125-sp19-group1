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

enum class Action { OPEN_BOX, CONSTRUCT_GATE, UNLOCK_JAIL, SWING_NET};

class Player
{
public:

	// Constructors
	Player();
	Player(int anID);
	Player(int anID, Location aLoc);

	// Getters
	ItemName	getInventory() const;
	bool		getInteracting() const;
	bool		getOpenJail() const;
	bool		getOpeningGate() const;
	Location	getLocation() const;
	ModelType	getModelType() const;
	bool		isChef() const;
	bool		getCaughtAnimal() const;
	bool		getIsCaught() const;
	int			getCaughtAnimalId() const;

	bool		isSelecting() const;


	// Setters
	void setInventory(ItemName anItem);
	void setModelType(ModelType type);
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
	void setInteracting(bool interact);
	void setOpenJail(bool interact);
	void setOpeningGate(bool status);
	void setIsCaught(bool caught);
	void setActionStartTime();
	void setStartJailTime();

	void toggleSelecting();


	bool inRange(Location & myLoc, Location & theirLoc);


	double getInteractingTime(int opt);

	std::string encodePlayerData(bool newPlayerInit);
	//	std::string encodePlayerData() const;
	void decodePlayerData(std::string key, std::string value);

	// Decode functions
	void addDecodeFunctions();
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeCakeStatus(std::string value);
	void decodeModelType(std::string value);

	// Encode functions
	void addEncodeFunctions();
	std::string encodeLocation();
	std::string encodeInventory();
	std::string encodeCakeStatus();
	std::string encodeModelType();

protected:
	Location	location;
	int			playerID;
	ItemName	inventory;
	bool		hasCake;
	ModelType	modelType;
	bool		interacting;
	bool		openingJail;
	bool		openingGate;
	bool		isCaught = false;
	std::chrono::time_point<std::chrono::system_clock> actionStartTime;
	std::chrono::time_point<std::chrono::system_clock> startJail;
	std::chrono::time_point<std::chrono::system_clock> startGate;

	std::map < std::string, bool> dirtyVariablesMap;
	using decodeFunctionType = void (Player::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	using encodeFunctionType = std::string(Player::*)();
	std::map<std::string, encodeFunctionType> encodingFunctions;


	bool selecting = true;
};
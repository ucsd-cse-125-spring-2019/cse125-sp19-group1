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
enum class ModelType { CHEF = 0, RACOON, CAT, DOG};

class Player
{
public:
	
	Player();
	Player(int anID);
	Player(int anID, Location aLoc);
	ItemName getInventory();
	void setInventory(ItemName anItem);
	Location getLocation();
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
	bool getInteracting();
	void setInteracting();
	bool getOpenJail();
	void setOpenJail();
	bool getOpenGate();
	void setOpenGate();
	ModelType getModelType();
	bool getIsChef();
	bool getCaughtAnimal();
	void setCaughtAnimal();
	bool getIsCaught();
	void setIsCaught();

	bool inRange(Location & myLoc, Location & theirLoc);

	void setStartTime();
	void setStartJailTime();
	double checkProgress(int opt);

	std::string encodePlayerData();
	void decodePlayerData(std::string key, std::string value);

	using decodeFunctionType =  void (Player::*)(std::string value);
	//std::map<std::string, decodeFunctionType> encodingFunctions;
	std::map<std::string, decodeFunctionType> decodingFunctions;

	void addDecodeFunctions();
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeCakeStatus(std::string value);
	void decodeChefStatus(std::string value);
	void decodeModelType(std::string value);

protected:
	Location	location;
	int			playerID;
	ItemName	inventory;
	bool		hasCake;
	bool		isChef;
	ModelType	modelType;
	bool		interacting;
	bool		openingJail;
	bool		openingGate;
	bool		caughtAnimal = false;
	bool		isCaught = false;
	int			radius = 10;
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> startJail;
	std::chrono::time_point<std::chrono::system_clock> startGate;

};
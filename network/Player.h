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
	ItemName getInventory() const;
	void setInventory(ItemName anItem);
	Location getLocation() const;
	void setLocation(float argX, float argY, float argZ);
	void setLocation(Location aLoc);
/*	bool getInteracting();
	void setInteracting(bool interact);
	bool getOpenJail();
	void setOpenJail(bool interact);
	bool getOpeningGate();
	void setOpeningGate(bool status);
	ModelType getModelType();
	bool getIsChef();
	bool getCaughtAnimal();
	void setCaughtAnimal(bool caught);
	bool getIsCaught();
	void setIsCaught(bool caught);
	int getCaughtAnimalId();
	void setCaughtAnimalId(int id);
*/
	bool getInteracting() const;
	void setInteracting();
	bool getOpenJail() const;
	void setOpenJail();
	bool getOpenGate() const;
	void setOpenGate();
	ModelType getModelType() const;
	bool getIsChef() const;
	bool getCaughtAnimal() const;
	void setCaughtAnimal();
	bool getIsCaught() const;
	void setIsCaught();

	bool inRange(Location & myLoc, Location & theirLoc);

	void setStartTime();
	void setStartJailTime();
	double checkProgress(int opt);

	std::string encodePlayerData(bool newPlayerInit) const;
//	std::string encodePlayerData() const;

	void decodePlayerData(std::string key, std::string value);

	using decodeFunctionType =  void (Player::*)(std::string value);
	std::map<std::string, decodeFunctionType> decodingFunctions;

	using encodeFunctionType = std::string (Player::*)();
	std::map<std::string, encodeFunctionType> encodingFunctions;
	//std::map<std::string, decodeFunctionType> encodingFunctions;

	std::map < std::string, bool> dirtyVariablesMap;

	void addDecodeFunctions();
	void decodeLocation(std::string value);
	void decodeInventory(std::string value);
	void decodeCakeStatus(std::string value);
	void decodeChefStatus(std::string value);
	void decodeModelType(std::string value);

	void addEncodeFunctions();
	std::string encodeLocation();
	std::string encodeInventory();
	std::string encodeCakeStatus();
	std::string encodeChefStatus();
	std::string encodeModelType();

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
	int			caughtAnimalId;
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> startJail;
	std::chrono::time_point<std::chrono::system_clock> startGate;

};
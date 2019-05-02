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

	void setStartTime();
	double checkBoxProgress();

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
	ItemName		inventory;
	bool		hasCake;
	bool		isChef;
	ModelType	modelType;
	bool		interacting;
	std::chrono::time_point<std::chrono::system_clock> start;
};
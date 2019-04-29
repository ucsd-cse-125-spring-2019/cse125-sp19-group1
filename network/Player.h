#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>

enum class Item { EMPTY = 0, KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE };

enum class ModelType { CHEF = 0, RACOON, CAT, DOG};

struct Location
{
public:
	Location(float argX = 10.0f, float argY = 0.0f, float argZ = 10.0f) : x(argX), y(argY), z(argZ) {}
	//Location(Location & aCopy) : x(aCopy.getX()), y(aCopy.getY()), z(aCopy.getZ()) {}

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

	float setX(float argX) { x = argX; }
	float setY(float argY) { y = argY; }
	float setZ(float argZ) { z = argZ; }

	void update(float argX, float argY, float argZ)
	{
		x = argX;
		y = argY;
		z = argZ;
	}

protected:
	float x;
	float y;
	float z;
};

class Player
{
public:
	
	Player();
	Player(int anID);
	Player(int anID, Location aLoc);
	Item getInventory();
	void setInventory(Item anItem);
	Location getLocation();
	void setLocation(float argX, float argY, float argZ);
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
	Item		inventory;
	bool		hasCake;
	bool		isChef;
	ModelType	modelType;
	bool		interacting;
	std::chrono::time_point<std::chrono::system_clock> start;
};
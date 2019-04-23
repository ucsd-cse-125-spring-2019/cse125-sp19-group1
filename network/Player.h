#pragma once
#include <string>
#include <sstream>
#include <iostream>

struct Location
{
	Location(float argX = 0.0f, float argY = 0.0f, float argZ = 0.0f) : x(argX), y(argY), z(argZ) {}

	float x;
	float y;
	float z;

	void update(float argX, float argY, float argZ)
	{
		x = argX;
		y = argY;
		z = argZ;
	}
};

class Player
{
public:
	
	Player();
	Player(int anID);
	Player(int anID, Location aLoc);
	Location getLocation();
	void setLocation(float argX, float argY, float argZ);

	std::string encodePlayerData();

protected:
	Location	location;
	int			playerID;
};
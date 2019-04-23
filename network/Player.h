#pragma once
#include <string>
#include <sstream>
#include <iostream>

struct Location
{
public:
	Location(float argX = 0.0f, float argY = 0.0f, float argZ = 0.0f) : x(argX), y(argY), z(argZ) {}

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
	Location getLocation();
	void setLocation(float argX, float argY, float argZ);

	std::string encodePlayerData();

protected:
	Location	location;
	int			playerID;
};
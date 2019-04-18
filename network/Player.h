#pragma once
#include "Location.h"

struct Location
{
public:

	Location(float x = 0.0f, float y = 0.0f, float z = 0.0f);

	std::vector<float> getLocation();
	void setLocation(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
protected:
	std::vector<float> loc;

};
class Player
{
public:
	
	Player();
	Player(Location aLoc);
	Location getLocation();

protected:
	Location loc;
};
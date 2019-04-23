#pragma once

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
	Player(Location aLoc);
	Location getLocation();
	void setLocation(float argX, float argY, float argZ);

protected:
	Location myLocation;
};
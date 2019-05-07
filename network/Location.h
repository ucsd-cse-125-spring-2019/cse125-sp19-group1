#pragma once

struct Location
{
public:
	Location(float argX = 10.0f, float argY = 0.0f, float argZ = 10.0f) : x(argX), y(argY), z(argZ) {}
	
	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

	void setX(float argX) { x = argX; }
	void setY(float argY) { y = argY; }
	void setZ(float argZ) { z = argZ; }

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

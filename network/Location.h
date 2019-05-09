#pragma once

struct Location
{
public:
	Location(float argX = 10.0f, float argY = 0.0f, float argZ = 10.0f) : x(argX), y(argY), z(argZ) {}
	
	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }

	void setX(float argX) { x = argX; }
	void setY(float argY) { y = argY; }
	void setZ(float argZ) { z = argZ; }

	void update(float argX, float argY, float argZ)
	{
		x = argX;
		y = argY;
		z = argZ;
	}

	bool operator ==(const Location & loc) const
	{
		return x == loc.x && y == loc.y && z == loc.z;
	}

protected:
	float x;
	float y;
	float z;
};

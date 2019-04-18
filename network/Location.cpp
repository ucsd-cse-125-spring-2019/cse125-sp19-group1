#include "Location.h"

// Initializes location based on input
Location::Location(float x = 0.0f, float y = 0.0f, float z = 0.0f) : loc(std::vector<float>{x, y, z}) {}

// Returns location float vector
std::vector<float> Location::getLocation() { return loc; }

// Sets the location
void Location::setLocation(float x, float y, float z)
{
	loc[0] = x;
	loc[1] = y;
	loc[2] = z;
}

// Gets individual x, y, z values
float Location::getX() { return loc[0]; }
float Location::getY() { return loc[1]; }
float Location::getZ() { return loc[2]; }


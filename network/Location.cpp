#include "Location.h"

// Default constructor initializes location at (0, 0, 0)
Location::Location() : loc(std::vector<float>{0.0f, 0.0f, 0.0f}) {}

// Initializes location based on input
Location::Location(float x, float y, float z) : loc(std::vector<float>{x, y, z}) {}

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


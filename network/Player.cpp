#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; }
Player::Player(int anID) : playerID(anID), inventory("empty"), hasCake(false)
{
	location = Location();
	
}

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory("empty"), hasCake(false)
{
}

Location Player::getLocation() { return location; }

void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
}

std::string Player::encodePlayerData()
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;

	return encodedData.str();
}

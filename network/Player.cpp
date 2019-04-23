#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "default constructor called\n"; }
Player::Player(int anID) : playerID(anID)
{
	location = Location();
}

Player::Player(int anID, Location aLoc) : playerID(anID)
{
	location = aLoc;
}

Location Player::getLocation() { return location; }

void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
}

std::string Player::encodePlayerData()
{
	std::stringstream encoding;
	encoding << "client: " << playerID << std::endl;
	encoding << "location: " << location.x << " " << location.y << " " << location.z << std::endl;

	return encoding.str();
}

#include "Player.h"

Player::Player()
{
	myLocation = Location();
}

Player::Player(Location aLoc)
{
	myLocation = aLoc;
}

Location Player::getLocation() { return myLocation; }

void Player::setLocation(float argX, float argY, float argZ)
{
	myLocation.update(argX, argY, argZ);
}

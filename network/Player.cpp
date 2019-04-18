#include "Player.h"

Player::Player()
{
	loc = Location();
}

Player::Player(Location aLoc)
{
	loc = aLoc;
}

Location Player::getLocation() { return loc; }

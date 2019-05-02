#pragma once
#include "Location.h"

enum class ItemName { EMPTY = 0, KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE };

struct Item {

public:

	Item() : name(ItemName::EMPTY), holdStatus(false), location(Location()), droppedTime(0) {}
	Item(ItemName anItem, Location aLoc) :name(anItem), location(aLoc), holdStatus(false), droppedTime(0) {}

	ItemName getName() { return name; }
	bool isHeld() { return holdStatus; }
	Location getLocation() { return location; }
	int getDroppedTime() { return droppedTime; }

	void setLocation(Location aLoc) { location = aLoc; }
	void setDroppedTime(int aTime) { droppedTime = aTime; }

protected:
	ItemName name;
	bool holdStatus;
	Location location;
	int droppedTime;
};
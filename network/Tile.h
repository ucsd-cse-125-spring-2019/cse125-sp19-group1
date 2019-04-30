#pragma once

#include "Player.h"

class Tile
{
public:
	bool hasBox();
	void setItem(ItemName anItem);
	void setBoxStatus(bool boxStatus);

	int getWallLayout();
protected:
	bool boxStatus;
	ItemName itemName;
	bool gateStatus;
	int wallLayout;
};
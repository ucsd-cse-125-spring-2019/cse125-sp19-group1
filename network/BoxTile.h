#pragma once
#include "Tile.h"

class BoxTile : public Tile
{
public:
	
	BoxTile(int aWallLayout, int aHeight) : Tile(TileType::BOX, aWallLayout, aHeight), boxStatus(true)
	{
		
	}

	void setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; setDirty(); }
	bool hasBox() { return boxStatus; }

protected:
	bool boxStatus;
};
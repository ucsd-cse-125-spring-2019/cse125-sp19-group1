#pragma once
#include "Item.h"
#include <string>
#include <sstream>

enum class TileType {DEFAULT = 0, BOX, GATE, TABLE, KEY_DROP, JAIL, RAMP};

class Tile
{
public:

	Tile(int aWallLayout = 0, TileType aType = TileType::DEFAULT, bool aBoxStatus = false, ItemName anItem = ItemName::EMPTY, int aHeight = 0, bool aDirty=true);
	void setItem(ItemName anItem);
	void setBoxStatus(bool boxStatus);
	void setDirty();

	bool isDirty();
	bool hasBox(); 
	int getWall();
	int getHeight();
	ItemName getItem();
	TileType getTileType();

	std::string encodeTileData();
	void decodeTileData(std::string value);
protected:
	
	int wall;
	int height;
	TileType tileType;
	bool boxStatus;
	ItemName itemName;
	bool dirty;
};
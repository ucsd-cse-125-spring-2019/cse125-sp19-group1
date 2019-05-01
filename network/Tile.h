#pragma once
#include "Player.h"

enum class TileType {DEFAULT = 0, BOX, GATE, TABLE, KEY_DROP, JAIL, RAMP};

class Tile
{
public:

	Tile(int aWallLayout = 0, TileType aType = TileType::DEFAULT, bool aBoxStatus = false, ItemName anItem = ItemName::EMPTY, int aHeight = 0);
	void setItem(ItemName anItem);
	void setBoxStatus(bool boxStatus);

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
};
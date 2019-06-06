#pragma once
#include "Item.h"
#include <string>
#include <sstream>
#include "GameConfigs.h"

enum class TileType {DEFAULT = 0, BOX, GATE, OBJECT, KEY_DROP, JAIL, RAMP, HIDE};

class Tile
{
public:

	// Constructor with default values if no params set
	Tile(TileType aType = TileType::DEFAULT, int aWallLayout = 0, int aHeight = 0, ItemModelType anItem = ItemModelType::EMPTY);

	bool isDirty();
	int getWall();
	int getHeight();
	ItemModelType getItem();
	TileType getTileType();

	// Setters
	void setItem(ItemModelType anItem);
	void setDirty();
	void setWall(int val);

	// Encode/Decode functions - overridden in subclasses
	virtual std::string encodeTileData();
	virtual void decodeTileData(std::string & value);

	//Getters
	bool isDirty() const;
	int getWall() const;
	int getHeight() const;
	ItemModelType getItem() const;
	TileType getTileType() const;

//	std::string encodeTileData();
//	void decodeTileData(std::string value);
protected:

	// Variables sent to client
	TileType	tileType;
	int			wall;
	int			height;
	ItemModelType	itemName;
	
	// Additional variables
	bool		dirty;
	bool		bearBuff;
};
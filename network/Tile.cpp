#include "Tile.h"

//Tile::Tile(int aWallLayout, TileType aType, bool aBoxStatus, int aHeight, ItemName anItem,  bool aDirty) :
//	wall(aWallLayout), tileType(aType), boxStatus(aBoxStatus), itemName(anItem), height(aHeight), dirty(aDirty) {}

Tile::Tile(TileType aType, int aWallLayout, int aHeight) : 
	tileType(aType), wall(aWallLayout), height(aHeight), itemName(ItemName::EMPTY), dirty(true) {}

Tile::Tile(TileType aType, int aWallLayout, int aHeight, ItemName anItem) :
	tileType(aType), wall(aWallLayout), height(aHeight), itemName(anItem), dirty(true) {}



void Tile::setItem(ItemName anItem) { itemName = anItem; setDirty(); }
//void Tile::setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; setDirty(); }
void Tile::setDirty() { dirty = true; }

bool Tile::isDirty() { return dirty; }
//bool Tile::hasBox() { return boxStatus; }
int Tile::getWall() { return wall; }
ItemName Tile::getItem() { return itemName; }
TileType Tile::getTileType() { return tileType; }
int Tile::getHeight() { return height; }

std::string Tile::encodeTileData()
{
	std::stringstream encodedData;

	encodedData 
		<< static_cast<int>(tileType) << " "
		<< wall << " "
		<< height << " "
	//	<< boxStatus << " "
		<< static_cast<int>(itemName);

	dirty = false;
	return encodedData.str();
}
void Tile::decodeTileData(std::string & value)
{
	std::stringstream valueStream(value);
	std::string wallLayout_str, height_str, tileType_str, boxStatus_str, itemName_str;

	valueStream 
		>> tileType_str
		>> wallLayout_str 
		>> height_str 
//		>> boxStatus_str 
		>> itemName_str;

	tileType = static_cast<TileType>(std::stoi(tileType_str));
	wall = std::stoi(wallLayout_str);
	height = std::stoi(height_str);
	//bool boxStatus = boxStatus_str == "1";
	itemName = static_cast<ItemName>(std::stoi(itemName_str));

	value = valueStream.str();
}
#include "Tile.h"

Tile::Tile(int aWallLayout, TileType aType, bool aBoxStatus, ItemName anItem, int aHeight, bool aDirty) :
	wall(aWallLayout), tileType(aType), boxStatus(aBoxStatus), itemName(anItem), height(aHeight), dirty(aDirty) {}

void Tile::setItem(ItemName anItem) { itemName = anItem; }
void Tile::setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; }
void Tile::setDirty() { dirty = true; }

bool Tile::isDirty() { return dirty; }
bool Tile::hasBox() { return boxStatus; }
int Tile::getWall() { return wall; }
ItemName Tile::getItem() { return itemName; }
TileType Tile::getTileType() { return tileType; }
int Tile::getHeight() { return height; }

std::string Tile::encodeTileData()
{
	std::stringstream encodedData;

	encodedData << wall << " "
		<< height << " "
		<< static_cast<int>(tileType) << " "
		<< boxStatus << " "
		<< static_cast<int>(itemName);

	dirty = false;
	return encodedData.str();
}
void Tile::decodeTileData(std::string value)
{
	std::stringstream valueStream(value);
	std::string wallLayout_str, height_str, tileType_str, boxStatus_str, itemName_str;

	valueStream >> wallLayout_str >> height_str >> tileType_str >> boxStatus_str >> itemName_str;

	int wallLayout = std::stoi(wallLayout_str);
	int height = std::stoi(height_str);
	TileType tileType = static_cast<TileType>(std::stoi(tileType_str));
	bool boxStatus = boxStatus_str == "1";
	ItemName itemName = static_cast<ItemName>(std::stoi(itemName_str));
}
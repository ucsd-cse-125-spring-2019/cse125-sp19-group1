#include "Tile.h"

Tile::Tile(int aWallLayout, TileType aType, bool aBoxStatus, int aHeight, ItemName anItem,  bool aDirty) :
	wall(aWallLayout), tileType(aType), boxStatus(aBoxStatus), itemName(anItem), height(aHeight), dirty(aDirty) {}

void Tile::setItem(ItemName anItem) { itemName = anItem; setDirty(); }
void Tile::setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; setDirty(); }
void Tile::setDirty() { dirty = true; }

bool Tile::isDirty() const { return dirty; }
bool Tile::hasBox() const { return boxStatus; }
int Tile::getWall() const { return wall; }
ItemName Tile::getItem() const { return itemName; }
TileType Tile::getTileType() const { return tileType; }
int Tile::getHeight() const { return height; }

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
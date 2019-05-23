#include "Tile.h"

// Default constructor
Tile::Tile(TileType aType, int aWallLayout, int aHeight, ItemModelType anItem) :
	tileType(aType), wall(aWallLayout), height(aHeight), itemName(anItem), dirty(true) {}

// Getters
TileType	Tile::getTileType() { return tileType; }
int			Tile::getWall()		{ return wall; }
int			Tile::getHeight()	{ return height; }
ItemModelType	Tile::getItem()		{ return itemName; }
bool		Tile::isDirty()		{ return dirty; }

// Setters
void Tile::setItem(ItemModelType anItem) { itemName = anItem; setDirty(); }
void Tile::setDirty() { dirty = true; }

bool Tile::isDirty() const { return dirty; }
//bool Tile::hasBox() const { return boxStatus; }
int Tile::getWall() const { return wall; }
ItemModelType Tile::getItem() const { return itemName; }
TileType Tile::getTileType() const { return tileType; }
int Tile::getHeight() const { return height; }

// Encode function
std::string Tile::encodeTileData()
{
	std::stringstream encodedData;

	// Encode class variables into the stringstream
	encodedData 
		<< static_cast<int>(tileType) << " "
		<< wall << " "
		<< height << " "
		<< static_cast<int>(itemName);

	// Update dirty status
	dirty = false;

	return encodedData.str();
}

// Decode function
void Tile::decodeTileData(std::string & value)
{
	std::stringstream valueStream(value);
	std::string tileType_str;
	std::string wallLayout_str;
	std::string height_str;
	std::string itemName_str;

	// Get values from the stream
	valueStream
		>> tileType_str
		>> wallLayout_str 
		>> height_str 
		>> itemName_str;

	// Update class variables
	tileType = static_cast<TileType>(std::stoi(tileType_str));
	wall = std::stoi(wallLayout_str);
	height = std::stoi(height_str);
	itemName = static_cast<ItemModelType>(std::stoi(itemName_str));

	// Updates value string to hold the remaining unparsed values
	std::getline(valueStream, value);
	// Removes the extra whitespace at the beginning
	value = value.erase(0, 1);
}
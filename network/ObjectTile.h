#pragma once
#include "Tile.h"
#include "../graphics/GraphicsClient/ItemModelType.h"

//enum class ItemModelType { TABLE=1, STOVE, CHAIR};

class ObjectTile : public Tile
{
public:

	// Default constructor
	ObjectTile(ItemModelType anObj = ItemModelType::table, int aWallLayout = 0, int aHeight = 0) : Tile(TileType::OBJECT, aWallLayout, aHeight), objectType(anObj) {}

	// Getter
	ItemModelType getObjectType() { return objectType; }
	// Setter

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< static_cast<int>(objectType);

		return encodedData.str();
	}

	// Decode function
	virtual void decodeTileData(std::string & value)
	{
		// Call base class decode function
		Tile::decodeTileData(value);

		// Create a stream for the remaining values
		std::stringstream valueStream(value);
		std::string objectType_str;

		// Get values from the stream
		valueStream
			>> objectType_str;

		// Update class variables
		objectType = static_cast<ItemModelType>(std::stoi(objectType_str));
	}

protected:
	ItemModelType objectType;
};
#pragma once
#include "Tile.h"

class BoxTile : public Tile
{
public:
	
	// Default constructor
	BoxTile(int aWallLayout = 0, int aHeight = 0) : Tile(TileType::BOX, aWallLayout, aHeight), boxStatus(true) {}

	// Getter
	bool hasBox() { return boxStatus; }
	// Setter
	void setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; setDirty(); }

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< boxStatus;

		return encodedData.str();
	}

	// Decode function
	virtual void decodeTileData(std::string & value)
	{
		// Call base class decode function
		Tile::decodeTileData(value);

		// Create a stream for the remaining values
		std::stringstream valueStream(value);
		std::string boxStatus_str;

		// Get values from the stream
		valueStream
			>> boxStatus_str;

		// Update class variables
		boxStatus = boxStatus_str == "1";
	}

protected:
	bool boxStatus;
};
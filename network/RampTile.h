#pragma once
#include "Tile.h"

enum class Orientation { NORTH = 4, SOUTH = 2, EAST = 1, WEST = 8};

class RampTile : public Tile
{
public:

	// Default constructor
	RampTile(Orientation aRampDir = Orientation::NORTH, int aWallLayout = 0, int aHeight = 0) : Tile(TileType::RAMP, aWallLayout, aHeight), rampDirection(aRampDir) {}

	// Getter
	Orientation getRampDirection() const { return rampDirection; }
	// Setter

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< static_cast<int>(rampDirection);

		return encodedData.str();
	}

	// Decode function
	virtual void decodeTileData(std::string & value)
	{
		// Call base class decode function
		Tile::decodeTileData(value);

		// Create a stream for the remaining values
		std::stringstream valueStream(value);
		std::string rampDirection_str;

		// Get values from the stream
		valueStream
			>> rampDirection_str;

		// Update class variables
		rampDirection = static_cast<Orientation>(rampDirection);
	}

protected:
	Orientation rampDirection;
};
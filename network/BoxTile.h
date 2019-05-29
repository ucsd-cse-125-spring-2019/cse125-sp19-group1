#pragma once
#include "Tile.h"

class BoxTile : public Tile
{
public:
	
	// Default constructor
	BoxTile(int aWallLayout = 0, int aHeight = 0) : Tile(TileType::BOX, aWallLayout, aHeight), boxStatus(true), keyBoxStatus(false) {}

	// Getter
	bool hasBox() { return boxStatus; }
	bool isKeyBox() { return keyBoxStatus; }
	double getBoxRespawnTime()
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - respawnStartTime;
		return elapsed_seconds.count();
	}
	// Setter
	void setBoxStatus(bool aBoxStatus) { boxStatus = aBoxStatus; setDirty(); }
	void setBoxRespawnTimestamp() { respawnStartTime = std::chrono::system_clock::now(); }
	void setKeyBox(bool status) { keyBoxStatus = status; }

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
	bool keyBoxStatus;
	std::chrono::time_point<std::chrono::system_clock> respawnStartTime;
};
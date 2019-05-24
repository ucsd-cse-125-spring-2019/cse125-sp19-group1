#pragma once
#include "Tile.h"
#include <vector>

enum class Key { KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9 };

class KeyDropTile : public Tile
{
public:

	// Default constructor
	KeyDropTile(std::vector<Key> aKeys = {}, int num = 0, int aWallLayout = 0, int aHeight = 0) :
		Tile(TileType::KEY_DROP, aWallLayout, aHeight), gateNum(num), keyProgress(0), validKeys(aKeys) {}

	// Getters
	int			getGateNum() { return gateNum; }
	//int			getKeyProgress() { return keyProgress; }
	//bool		hasAllKeys() { return keyProgress == 3; }

	// Setters

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< gateNum << " "
			<< keyProgress;

		return encodedData.str();
	}

	// Decode function
	virtual void decodeTileData(std::string & value)
	{
		// Call base class decode function
		Tile::decodeTileData(value);

		// Create a stream for the remaining values
		std::stringstream valueStream(value);
		std::string gateNum_str;
		std::string keyProgress_str;

		// Get values from the stream
		valueStream
			>> gateNum_str
			>> keyProgress_str;

		// Update class variables
		gateNum = std::stoi(gateNum_str);
		keyProgress = std::stoi(keyProgress_str);
	}

	// Additional functions

	// Checks if the key is valid for this gate
	bool isValidKey(Key aKey)
	{
		return std::find(validKeys.begin(), validKeys.end(), aKey) != validKeys.end();
	}

	// Updates progress and removes the key so that duplicate keys cannot be used
	void updateKeyProgress(Key aKey) {
		if (isValidKey(aKey))
		{
			validKeys.erase(std::find(validKeys.begin(), validKeys.end(), aKey));
		}
	}

protected:

	// Variable sent to client
	int gateNum;
	int keyProgress;

	// Additional variables
	std::vector<Key> validKeys;
};
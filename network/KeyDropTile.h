#pragma once
#include "Tile.h"
#include <vector>

//enum class ItemModelType { KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9 };

class KeyDropTile : public Tile
{
public:

	// Default constructor
	KeyDropTile(std::vector<ItemModelType> aKeys = {}, int num = 0, ItemModelType aModel = ItemModelType::apple, int aWallLayout = 0, int aHeight = 0) :
		Tile(TileType::KEY_DROP, aWallLayout, aHeight), gateNum(num), validKeys(aKeys), model(aModel) {}

	// Getters
	int			getGateNum() { return gateNum; }
	ItemModelType getModel() { return model; }
	std::vector<ItemModelType> getDepositedKeys() { return depositedKeys; }

	// Setters

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< gateNum << " "
			<< static_cast<int>(model);

		for (ItemModelType k : depositedKeys)
		{
			encodedData << " " << static_cast<int>(k);
		}

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
		std::string model_str;
		std::string depositedKey_str;

		// Get values from the stream
		valueStream
			>> gateNum_str
			>> model_str;

		depositedKeys.clear();
		while (valueStream >> depositedKey_str)
		{
			depositedKeys.push_back(static_cast<ItemModelType>(std::stoi(depositedKey_str)));
		}

		// Update class variables
		gateNum = std::stoi(gateNum_str);
		model = static_cast<ItemModelType>(std::stoi(model_str));
	}

	// Additional functions

	// Checks if the key is valid for this gate
	bool isValidKey(ItemModelType aKey)
	{
		return std::find(validKeys.begin(), validKeys.end(), aKey) != validKeys.end();
	}

	// Updates progress and removes the key so that duplicate keys cannot be used
	void updateKeyProgress(ItemModelType aKey) {
		if (isValidKey(aKey))
		{
			depositedKeys.push_back(aKey);
			setDirty();
			validKeys.erase(std::find(validKeys.begin(), validKeys.end(), aKey));
		}
	}

protected:

	// Variable sent to client
	int gateNum;
	ItemModelType model;

	// Additional variables
	std::vector<ItemModelType> validKeys;
	std::vector<ItemModelType> depositedKeys;
};
#pragma once
#include "Tile.h"
#include <vector>

//enum class Key { KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9 };

class GateTile : public Tile
{
public:
	
	// Default constructor
	//GateTile(std::vector<Key> aKeys = {}, int num = 0, int aWallLayout = 0, int aHeight = 0) :
	//	Tile(TileType::GATE, aWallLayout, aHeight), gateNum(num), keyProgress(0), currentConstructTime(0.0), validKeys(aKeys) {}
	GateTile(int num = 0, ItemModelType aModel = ItemModelType::apple, int aWallLayout = 0, int aHeight = 0) :
		Tile(TileType::GATE, aWallLayout, aHeight), gateNum(num), model(aModel), keyProgress(0), currentConstructTime(0.0) {}

	// Getters
	int			getGateNum() { return gateNum; }
	int			getKeyProgress() { return keyProgress; }
	double		getCurrentConstructTime() { return currentConstructTime; }
	bool		hasAllKeys() { return keyProgress >= 3; }
	bool		isOpen() { return currentConstructTime >= TIME_TO_CONSTRUCT_GATE; }
	ItemModelType getModel() { return model; }
	// Setters
	
	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " "
			<< gateNum << " "
			<< keyProgress << " "
			<< currentConstructTime << " "
			<< static_cast<int>(model);

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
		std::string currentConstructTime_str;
		std::string model_str;

		// Get values from the stream
		valueStream
			>> gateNum_str
			>> keyProgress_str
			>> currentConstructTime_str
			>> model_str;

		// Update class variables
		gateNum = std::stoi(gateNum_str);
		keyProgress = std::stoi(keyProgress_str);
		currentConstructTime = std::stof(currentConstructTime_str);
		model = static_cast<ItemModelType>(std::stoi(model_str));
	}

	// Additional functions

	//// Checks if the key is valid for this gate
	//bool isValidKey(Key aKey) 
	//{ 
	//	return std::find(validKeys.begin(), validKeys.end(), aKey) != validKeys.end();
	//}

	// Updates progress and removes the key so that duplicate keys cannot be used
	//void updateKeyProgress(Key aKey) {
	void updateKeyProgress() {
		//if (isValidKey(aKey))
		//{
			keyProgress++;
		//	validKeys.erase(std::find(validKeys.begin(), validKeys.end(), aKey));
			setDirty();
		//}
	}

	// Adds construction time for building the exit once alls keys are deposited
	void constructGate(double time)
	{
		currentConstructTime += time;
		setDirty();
	}

protected:
	
	// Variable sent to client
	int gateNum;
	int keyProgress;
	double currentConstructTime;
	ItemModelType model;

	// Additional variables
	//std::vector<Key> validKeys;
};
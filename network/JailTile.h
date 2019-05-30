#pragma once
#include "Tile.h"

class JailTile : public Tile
{
public:
	
	//Deafult Constructor
	JailTile(int aWallLayout = 0, int aHeight = 0) : Tile(TileType::JAIL, aWallLayout, aHeight), 
		capturedAnimal(-1), progress(0), jailStatus(false) {}

	//Getter
	bool hasJail() { return jailStatus; }
	//Setter
	void setJailStatus(bool aJailStatus) { jailStatus = aJailStatus; }

	int getCapturedAnimal() { return capturedAnimal; }
	int getProgress() { return progress; }
	bool isJailEmpty() { return (capturedAnimal == -1); };

	void placeAnimalInJail(int iter) { capturedAnimal = iter; };
	void unlockJail() { progress += 1; };
	void resetJail() 
	{ 
		capturedAnimal = -1; 
		progress = 0; 
	};

	//endcode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		encodedData << Tile::encodeTileData() 
			<< " " << jailStatus
			<< " " << progress
			<< " " << capturedAnimal;

		return encodedData.str();
	}

	//Decode function
	virtual void decodeTileData(std::string & value)
	{
		Tile::decodeTileData(value);
	
		std::stringstream valueStream(value);
		std::string jailStatus_str;
		std::string progress_str;
		std::string capturedAnimal_str;
		
		valueStream >> jailStatus_str >> progress_str >> capturedAnimal_str;

		jailStatus = jailStatus_str == "1";
		progress = std::stoi(progress_str);
		capturedAnimal = std::stoi(capturedAnimal_str);

	}

protected:
	bool jailStatus = false;
	int capturedAnimal = -1;
	int progress = 0;
};
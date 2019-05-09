#pragma once
#pragma once
#include "Tile.h"
#include <vector>


class HideTile : public Tile
{
public:

	// Default constructor
	HideTile(): animalHiding(-1) {}

	// Getters
	bool		checkHideTileEmpty() { return animalHiding == -1; }
	int			getAnimalHiding() { return animalHiding; }

	// Setters
	void		setAnimalHiding(int animal) { animalHiding = animal; }

	// Encode function
	virtual std::string encodeTileData()
	{
		std::stringstream encodedData;

		// Call base class encode function and encode member variables from this class to the stringstream
		encodedData << Tile::encodeTileData() << " " 
			<< animalHiding;

		return encodedData.str();
	}

	// Decode function
	virtual void decodeTileData(std::string & value)
	{
		// Call base class decode function
		Tile::decodeTileData(value);

		// Create a stream for the remaining values
		std::stringstream valueStream(value);
		std::string animalHiding_str;

		// Get values from the stream
		valueStream
			>> animalHiding_str;

		// Update class variables
		animalHiding = std::stoi(animalHiding_str);
		
	}

protected:

	// Variable sent to client
	int animalHiding;

};
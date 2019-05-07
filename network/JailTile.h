#pragma once
#include "Tile.h"

class JailTile : public Tile
{
public:
	JailTile() : capturedAnimal(-1), progress(0), empty(true) {}

	int getCapturedAnimal() { return capturedAnimal; }
	int getProgress() { return progress; }
	bool isJailEmpty() { return empty; };

	void placeAnimalInJail(int iter) { capturedAnimal = iter; };
	void unlockJail() { progress += 1; };
	void resetJail() { capturedAnimal = -1; empty = true; };

protected:
	int capturedAnimal;
	int progress;
	bool empty;
};
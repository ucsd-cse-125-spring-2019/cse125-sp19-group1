#pragma once
#include "Location.h"
#include <chrono>

enum class ItemName { EMPTY = 0, KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE = 100 };

struct Item {

public:

	Item() : name(ItemName::EMPTY), location(Location()), dropTime(std::chrono::system_clock::now()), spawnRowIndex(0), spawnColIndex(0) {}
	Item(ItemName anItem, Location aLoc) :name(anItem), location(aLoc), dropTime(std::chrono::system_clock::now()), spawnRowIndex(0), spawnColIndex(0) {}
	Item(ItemName anItem, int row, int col) :name(anItem), location(Location()), dropTime(std::chrono::system_clock::now()), spawnRowIndex(row), spawnColIndex(col), droppedRowIndex(row), droppedColIndex(col) {}

	ItemName getName() { return name; }
	Location getLocation() { return location; }
	void getSpawnLocation(int & row, int & col) { row = spawnRowIndex; col = spawnColIndex; }
	void getDropLocation(int & row, int & col) { row = droppedRowIndex; col = droppedColIndex; }
	double getDropDuration() 
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - dropTime;
		return elapsed_seconds.count();
	}

	void setLocation(Location aLoc) { location = aLoc; }
	void setDropTime() { dropTime = std::chrono::system_clock::now(); }
	void setIndices(int row, int col) { spawnRowIndex = row; spawnColIndex = col; }
	void setDroppedIndices(int row, int col) { droppedRowIndex = row; droppedColIndex = col; }


	bool hasBeenMoved()
	{
		return droppedRowIndex != spawnRowIndex || droppedColIndex != spawnColIndex;
	}

	void resetDropStatus()
	{
		droppedRowIndex = spawnRowIndex;
		droppedColIndex = spawnColIndex;
	}

protected:
	ItemName name;
	Location location;
	int spawnRowIndex;
	int spawnColIndex;
	int droppedRowIndex;
	int droppedColIndex;
	std::chrono::time_point<std::chrono::system_clock> dropTime;


};
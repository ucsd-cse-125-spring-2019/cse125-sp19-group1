#pragma once
#include "Location.h"
#include <chrono>

enum class ItemName { EMPTY = 0, KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE = 100 };

struct Item {

public:

	Item() : name(ItemName::EMPTY), holdStatus(false), location(Location()), dropTime(std::chrono::system_clock::now()), spawnRowIndex(0), spawnColIndex(0), dropped(false) {}
	Item(ItemName anItem, Location aLoc) :name(anItem), location(aLoc), holdStatus(false), dropTime(std::chrono::system_clock::now()), spawnRowIndex(0), spawnColIndex(0), dropped(false) {}
	Item(ItemName anItem, int row, int col) :name(anItem), location(Location()), holdStatus(false), dropTime(std::chrono::system_clock::now()), spawnRowIndex(row), spawnColIndex(col), dropped(false), droppedRowIndex(row), droppedColIndex(col) {}

	ItemName getName() { return name; }
	bool isHeld() { return holdStatus; }
	Location getLocation() { return location; }
	void getStartLocation(int & row, int & col) { row = spawnRowIndex; col = spawnColIndex; }
	void getDropLocation(int & row, int & col) { row = droppedRowIndex; col = droppedColIndex; }
	//int getDropTime() { return dropTime; }
	double getDropDuration() 
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - dropTime;
		return elapsed_seconds.count();
	}

	void setLocation(Location aLoc) { location = aLoc; }
	//void setDropTime(int aTime) { dropTime = aTime; }
	void setDropTime() { dropTime = std::chrono::system_clock::now(); }
	void setIndices(int row, int col) { spawnRowIndex = row; spawnColIndex = col; }
	void setDroppedIndices(int row, int col) { droppedRowIndex = row; droppedColIndex = col; }
	void setHoldStatus(bool status) { holdStatus = status; }

	bool wasDropped() { return dropped; }
	void setDropped(bool status) { dropped = status; }

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
	bool holdStatus;
	Location location;
	int spawnRowIndex;
	int spawnColIndex;
	int droppedRowIndex;
	int droppedColIndex;
	//int dropTime;
	std::chrono::time_point<std::chrono::system_clock> dropTime;

	bool dropped;

};
#pragma once
#include "Location.h"

enum class ItemName { EMPTY = 0, KEY1 = 1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, CAKE = 100 };

struct Item {

public:

	Item() : name(ItemName::EMPTY), holdStatus(false), location(Location()), droppedTime(0), rowIndex(0), colIndex(0), dropped(false) {}
	Item(ItemName anItem, Location aLoc) :name(anItem), location(aLoc), holdStatus(false), droppedTime(0), rowIndex(0), colIndex(0), dropped(false) {}
	Item(ItemName anItem, int row, int col) :name(anItem), location(Location()), holdStatus(false), droppedTime(0), rowIndex(row), colIndex(col), dropped(false) {}

	ItemName getName() { return name; }
	bool isHeld() { return holdStatus; }
	Location getLocation() { return location; }
	void getStartLocation(int & row, int & col) { row = rowIndex; col = colIndex; }
	void getDropLocation(int & row, int & col) { row = droppedRowIndex; col = droppedColIndex; }
	int getDroppedTime() { return droppedTime; }

	void setLocation(Location aLoc) { location = aLoc; }
	void setDroppedTime(int aTime) { droppedTime = aTime; }
	void setIndices(int row, int col) { rowIndex = row; colIndex = col; }
	void setDroppedIndices(int row, int col) { droppedRowIndex = row; droppedColIndex = col; }
	void setHoldStatus(bool status) { holdStatus = status; }

	bool wasDropped() { return dropped; }
	void setDropped(bool status) { dropped = status; }

	bool hasBeenMoved(int row, int col)
	{
		return row != rowIndex || col != colIndex;
	}

protected:
	ItemName name;
	bool holdStatus;
	Location location;
	int rowIndex;
	int colIndex;
	int droppedRowIndex;
	int droppedColIndex;
	int droppedTime;
	bool dropped;
};
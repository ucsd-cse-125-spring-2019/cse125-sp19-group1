#pragma once
#include <vector>
#include "Location.h"

struct Jail
{
public:
	Jail();
	bool isJailEmpty();

protected:
	Location location;
	std::vector<int> capturedAnimals;
	int progress;





};
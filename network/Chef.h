#pragma once
#include "Player2.h"

class Chef : public Player
{
public:
	void setCaughtAnimalId(int id);
	void setCaughtAnimal(bool caught);

protected:
	int			caughtAnimalId;
	bool		caughtAnimal = false;
	int			radius = 10;

};
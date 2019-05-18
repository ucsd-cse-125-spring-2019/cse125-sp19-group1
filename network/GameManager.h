#pragma once
#include "GameData.h"

class GameManager
{
public:
	GameManager();

	void updateForwardEvent(int id);
	void updateBackwardEvent(int id);
	void updateLeftEvent(int id);
	void updateRightEvent(int id);
	void updateHeight(int id);
	void updateCollision(int id);
	void updatePlayerCollision(int id, int dir);
	//void resetGame();

	bool gameHasStarted();
	bool gameIsOver()

protected:
	GameData * gameData;
	bool gameStarted = true;
	bool allPlayersReady;

	bool moveForward, moveBackward, moveLeft, moveRight;
	unsigned int SPEED = 2;
	bool chefWin = false;
	bool animalWin = false;
};
#pragma once
#include "GameData.h"
#include <string>

class GameManager
{
public:
	GameManager();

	void updateMovement(int dir, int id);
	void updateForwardEvent(int id);
	void updateBackwardEvent(int id);
	void updateLeftEvent(int id);
	void updateRightEvent(int id);
	void updateHeight(int id);
	void updateCollision(int id);
	void updatePlayerCollision(int id, int dir);
	void togglePlayerReady(int id);

	void updatePlayerModel(int id, ModelType model);
	void updatePlayerFacingDirection(int id);
	void resetPlayerDirectionFlags();
	void resetGame();

	bool gameHasStarted();
	bool getAllPlayersReady();
	bool gameIsOver();

	std::string encodeGameData(bool newPlayerInit);

	void addNewPlayer(int anID, Location aLoc, ClientType type);
	void removePlayer(int anID, ClientType type);

	void update();

	bool isCountingDown();
	void startCountdown();

	void interactEvent(int id);
	void releaseEvent(int id);
	void dropEvent(int id);
	void hideEvent(int id);
	void swingAction(int id);
protected:
	GameData * gameData;
	bool gameStarted = true;
	bool allPlayersReady;

	bool moveForward, moveBackward, moveLeft, moveRight;
	unsigned int SPEED = 2;
	bool chefWin = false;
	bool animalWin = false;	
};
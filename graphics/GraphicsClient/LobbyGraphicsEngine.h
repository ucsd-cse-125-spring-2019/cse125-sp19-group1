#pragma once
#include "TwoDeeGraphicsEngine.h"

#define LOBBY_MAX_PLAYERS 4

struct LobbySprite;
class FBXObject;

class LobbyGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	LobbyGraphicsEngine();
	~LobbyGraphicsEngine();

	virtual void MainLoopBegin();
	virtual void MainLoopEnd();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MainLoopCallback(GLFWwindow * window);

protected:
	void DrawSprite(LobbySprite &sprite, float designX, float designY, float alpha = 1.f);

	bool startEnabled = false;

	float warningAlpha;
	FBXObject *warningObject;

	float targetY[LOBBY_MAX_PLAYERS] = { 0.f };
	float playerX[LOBBY_MAX_PLAYERS] = { 0.f };
	float playerY[LOBBY_MAX_PLAYERS] = { 0.f };
};


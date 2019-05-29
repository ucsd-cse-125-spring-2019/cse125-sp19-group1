#pragma once
#include "TwoDeeGraphicsEngine.h"

#define LOBBY_MAX_PLAYERS 4

struct LobbySprite;

class LobbyGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	bool quit = false;

	LobbyGraphicsEngine();
	~LobbyGraphicsEngine();

	virtual void StartLoading();  // may launch a thread and return immediately
	virtual void CleanUp();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MainLoopCallback(GLFWwindow * window);

protected:
	void DrawSprite(LobbySprite &sprite, int designX, int designY, float alpha = 1.f);

	bool startEnabled = false;
	int myPlayerNum = 1;
	bool playerIsAnimal[LOBBY_MAX_PLAYERS] = { false };

	float targetY[LOBBY_MAX_PLAYERS] = { 0.f };
	float playerX[LOBBY_MAX_PLAYERS] = { 0.f };
	float playerY[LOBBY_MAX_PLAYERS] = { 0.f };
};


#pragma once

#include "AbstractGraphicsEngine.h"

class InGameGraphicsEngine : public AbstractGraphicsEngine
{
public:
	InGameGraphicsEngine();
	~InGameGraphicsEngine();

	virtual void StartLoading();  // may launch a thread and return immediately
	virtual void CleanUp();
	virtual void MainLoopBegin();
	virtual void MainLoopEnd();

	virtual void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void CursorPosCallback(GLFWwindow * window, double xpos, double ypos);
	virtual void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	virtual void MainLoopCallback(GLFWwindow * window);

	virtual bool ShouldFadeout();

protected:
	void InitCamera(const glm::vec3 &newPlayerPos);
	void MoveCamera(const glm::vec3 &newPlayerPos);
	void IdleCallback();
	void MovePlayers();
	void ReloadPlayers();

	bool quit = false;
	bool needsRenderingSetup = false;
};


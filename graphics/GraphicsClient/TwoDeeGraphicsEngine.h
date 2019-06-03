#pragma once
#include "AbstractGraphicsEngine.h"

class FBXObject;

FBXObject * createObjectForTexture(const char *texturePath);

class TwoDeeGraphicsEngine : public AbstractGraphicsEngine
{
public:
	TwoDeeGraphicsEngine();
	~TwoDeeGraphicsEngine();

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

	static GLuint retainShader();
	static void releaseShader();

protected:
	const char * backgroundFilename;

	static GLuint passthroughShaderProgram;
	static int passthroughShaderRefCount;

	glm::mat4 orthoProj;
	GLuint uAlpha;
	FBXObject *backgroundObj;
	bool quit;
};


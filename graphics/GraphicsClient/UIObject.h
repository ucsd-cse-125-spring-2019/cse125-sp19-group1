#ifndef UIOBJECT_H
#define UIOBJECT_H

#include "Core.h"
#include "UICanvas.h"
#include "FBXObject.h"
#include "shader.h"
#include "objloader.h"
#include "textureloader.h"
#include <string>

class UIObject
{
public:
	void draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::mat4 model);
	UIObject(const char * texPath);
	~UIObject();
private:

	GLuint textureID;
	static FBXObject * sharedAltCanvas;
	static int altCanvasRefCount;
};

#endif

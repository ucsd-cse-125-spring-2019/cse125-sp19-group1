#ifndef UIOBJECT_H
#define UIOBJECT_H

#include "Core.h"
#include "Skeleton.h"
#include "shader.h"
#include "objloader.h"
#include "textureloader.h"
#include <string>

class UIObject
{
public:
	UIObject(GLuint shaderProgram, const char * texPath);
	~UIObject();

	void draw( glm::vec2 position,
		glm::mat4 projection,
		glm::vec2 size = glm::vec2(100, 100), GLfloat rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));
private:

	GLfloat vertices[24] = {
-1,0,1,
1,0,1,
-1,2,1,
1,2,1,
-1,2,-1,
1,2,-1,
-1,0,-1,
1,0,-1
	};

	GLfloat uvs[24] = {
	16,17,18,
	19,8,9,
	10,11,20,
	21,22,23,
	12,13,14,
	15,0,1,
	2,3,4,
	5,6,7
	};

	int indices[24] = {
		0,1,3,
		-3,2,3,
		5,-5,4,
		5,7,-7,
		6,7,1,
		-1,1,7,
		5,-4,6,
		0,2,-5
	};

	GLuint VAO, VBO_V, VBO_UV, EBO;
	GLuint shaderProgram;
	GLuint texNum;
	void initRenderData();
};

#endif

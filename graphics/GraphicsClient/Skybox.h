#pragma once

#include "Core.h"

class Skybox
{
public:
	Skybox();
	~Skybox();

	void Draw(const glm::mat4 & P, const glm::mat4 & V);

private:
	GLuint shader;
	GLuint texture;
	GLuint skyboxVAO;
	GLuint skyboxVBO;
	GLint uProjection;
	GLint uModelview;

};


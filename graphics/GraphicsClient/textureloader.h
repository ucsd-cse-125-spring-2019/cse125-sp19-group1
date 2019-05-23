#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>


GLuint loadTexture(const char * textureName, 
	int *texWidth = nullptr, int *texHeight = nullptr, GLint filtering = GL_NEAREST);
unsigned char* loadPPM(const char* filename, int& width, int& height);
#endif


#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>


GLuint loadTexture(const char * textureName, int *texWidth = nullptr, int *texHeight = nullptr);
unsigned char* loadPPM(const char* filename, int& width, int& height);
#endif


#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <functional>

struct WicImage {
	void *textureBitmap = NULL;
	void *lock = NULL;
	unsigned width;
	unsigned height;
	void *buffer;

};

bool LoadWicImage(const wchar_t *filename, WicImage *image);
void ReleaseWicImage(const WicImage *image);

GLuint loadTexture(const char * textureName, 
	int *texWidth = nullptr, int *texHeight = nullptr, GLint filtering = GL_NEAREST);
unsigned char* loadPPM(const char* filename, int& width, int& height);
#endif


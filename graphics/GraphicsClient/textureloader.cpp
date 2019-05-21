#define _CRT_SECURE_NO_WARNINGS 1

#include "textureloader.h"
//#include "WICTextureLoader.h"
#include "lodepng.h"

#include <cstdlib>
#include <vector>

unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	size_t read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf_s(buf[0], "%s %s",  buf[1], BUFSIZE, buf[2], BUFSIZE);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}


GLuint loadTexture(const char * textureName)
{
	GLuint texture[1];     // storage for one texture

	// Create ID for texture
	glGenTextures(1, &texture[0]);

	const char * fileType = strrchr(textureName, '.');
	if (fileType == nullptr || *fileType != '.' || !strcmp(fileType + 1, "ppm")) {
		int twidth, theight;   // texture width/height [pixels]
		unsigned char* tdata;  // texture pixel data
							   // Load image file
		tdata = loadPPM(textureName, twidth, theight);
		//std::cerr << "Cat Image: " << twidth << " X " << theight << std::endl;
		if (tdata == NULL) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}

		// Set this texture to be the one we are working with
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
	}
	else {
		std::vector<unsigned char> bytes;
		unsigned width, height;

		if (lodepng::decode(bytes, width, height, textureName)) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}

		// Set this texture to be the one we are working with
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(bytes[0]));

		/*wchar_t filename[512];
		mbstowcs(filename, textureName, 512);

		if (WICTexImage2DFromFile(texture[0], 0, filename) < 0) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}*/


	}

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texture[0];
}
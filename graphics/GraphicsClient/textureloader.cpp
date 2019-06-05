#define _CRT_SECURE_NO_WARNINGS 1

#include "textureloader.h"
//#include "lodepng.h"

#include <cstdlib>
#include <vector>
#include <windows.h>
#include <wincodec.h>

#pragma comment(lib, "Windowscodecs.lib")
#pragma comment(lib, "Ole32.lib")

/*
 * Uses a modified version of William Atkins PNG loading code
 * https://github.com/WilliamAkins/2D-OpenGL-Scene/blob/master/texture_loader.cpp
 */


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

template <class T> static void SafeRelease(T *&ppT)
{
	if (ppT)
	{
		ppT->Release();
		ppT = NULL;
	}
}

//
// COM (Component Object Model) initialisation and shutdown
//

HRESULT initCOM(void) {
	return CoInitialize(NULL);
}

void shutdownCOM(void) {

	// Notify COM we're done
	CoUninitialize();
}

// Windows Imaging Component factory class (singleton)
static IWICImagingFactory *wicFactory = NULL;

HRESULT getWICFormatConverter(IWICFormatConverter **formatConverter) {
	if (!formatConverter || !wicFactory)
		return E_FAIL;
	else
		return wicFactory->CreateFormatConverter(formatConverter);
}


// Load and return an IWICBitmap interface representing the image loaded from path.  No format conversion is done here - this is left to the caller so each delegate can apply the loaded image data as needed.
HRESULT loadWICBitmap(LPCWSTR path, IWICBitmap **bitmap) {
	if (!bitmap || !wicFactory)
		return E_FAIL;

	IWICBitmapDecoder *bitmapDecoder = NULL;
	IWICBitmapFrameDecode *imageFrame = NULL;
	IWICFormatConverter *formatConverter = NULL;

	*bitmap = NULL;

	// Create image decoder
	HRESULT hr = wicFactory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);

	// Validate number of frames

	UINT numFrames = 0;

	if (SUCCEEDED(hr)) {

		hr = bitmapDecoder->GetFrameCount(&numFrames);
	}

	if (SUCCEEDED(hr) && numFrames>0) {

		// Decode first image frame (default to first frame - for animated types add parameters to select frame later!)
		hr = bitmapDecoder->GetFrame(0, &imageFrame);
	}

	if (SUCCEEDED(hr)) {

		hr = wicFactory->CreateFormatConverter(&formatConverter);
	}

	WICPixelFormatGUID pixelFormat;

	if (SUCCEEDED(hr)) {

		// Check we can convert to the required format GUID_WICPixelFormat32bppPBGRA			
		hr = imageFrame->GetPixelFormat(&pixelFormat);
	}

	BOOL canConvert = FALSE;

	if (SUCCEEDED(hr)) {
		hr = formatConverter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppPBGRA, &canConvert);
	}

	if (SUCCEEDED(hr) && canConvert == TRUE) {

		hr = formatConverter->Initialize(
			imageFrame,						// Input source to convert
			GUID_WICPixelFormat32bppPBGRA,	// Destination pixel format
			WICBitmapDitherTypeNone,		// Specified dither pattern
			NULL,							// Specify a particular palette 
			0.f,							// Alpha threshold
			WICBitmapPaletteTypeCustom		// Palette translation type
		);
	}

	if (SUCCEEDED(hr)) {

		// Convert and create bitmap from converter
		hr = wicFactory->CreateBitmapFromSource(formatConverter, WICBitmapCacheOnDemand, bitmap);
	}

	// Cleanup
	SafeRelease(formatConverter);
	SafeRelease(imageFrame);
	SafeRelease(bitmapDecoder);

	// Return result
	return hr;
}


void ReleaseWicImage(const WicImage *image) {
	if (!image) return;

	((IWICBitmapLock *)image->lock)->Release();
	((IWICBitmap *)image->textureBitmap)->Release();
}

bool LoadWicImage(const wchar_t *filename, WicImage *image) {
	HRESULT		hr;

	// On first call instantiate WIC factory class
	if (!wicFactory) {

		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

		// Validate wicFactory before proceeding
		if (!SUCCEEDED(hr))
			return -1;
	}


	IWICBitmap			*textureBitmap = NULL;
	IWICBitmapLock		*lock = NULL;

	hr = loadWICBitmap(filename, &textureBitmap);

	UINT w = 0, h = 0;

	if (SUCCEEDED(hr))
		hr = textureBitmap->GetSize(&w, &h);

	WICRect rect = { 0, 0, w, h };

	if (SUCCEEDED(hr)) {
		hr = textureBitmap->Lock(&rect, WICBitmapLockRead, &lock);
	}

	UINT bufferSize = 0;
	BYTE *buffer = NULL;

	if (SUCCEEDED(hr))
		hr = lock->GetDataPointer(&bufferSize, &buffer);

	if (SUCCEEDED(hr)) {
		image->textureBitmap = textureBitmap;
		image->lock = lock;
		image->buffer = buffer;
		image->width = w;
		image->height = h;
	}

	return SUCCEEDED(hr);
}


GLuint wicLoadTexture(const std::wstring& filename, int *texWidth, int *texHeight, GLint filtering) {
	HRESULT		hr;

	// On first call instantiate WIC factory class
	if (!wicFactory) {

		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

		// Validate wicFactory before proceeding
		if (!SUCCEEDED(hr))
			return 0;
	}


	IWICBitmap			*textureBitmap = NULL;
	IWICBitmapLock		*lock = NULL;
	GLuint				newTexture = 0;

	hr = loadWICBitmap(filename.c_str(), &textureBitmap);

	UINT w = 0, h = 0;

	if (SUCCEEDED(hr))
		hr = textureBitmap->GetSize(&w, &h);

	WICRect rect = { 0, 0, w, h };

	if (SUCCEEDED(hr)) {
		hr = textureBitmap->Lock(&rect, WICBitmapLockRead, &lock);
		if (texWidth) *texWidth = w;
		if (texHeight) *texHeight = h;
	}

	UINT bufferSize = 0;
	BYTE *buffer = NULL;

	if (SUCCEEDED(hr))
		hr = lock->GetDataPointer(&bufferSize, &buffer);

	if (SUCCEEDED(hr)) {

		glGenTextures(1, &newTexture);
		glBindTexture(GL_TEXTURE_2D, newTexture);

		// Note: GL_BGRA format used - input image format converted to GUID_WICPixelFormat32bppPBGRA for consistent interface with OpenGL texture setup
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
	}

	SafeRelease(lock);
	SafeRelease(textureBitmap);


	// Setup default texture properties
	if (newTexture) {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return newTexture;
}


GLuint loadTexture(const char * textureName, int *texWidth, int *texHeight, GLint filtering)
{
	const char * fileType = strrchr(textureName, '.');
	if (fileType == nullptr || !strcmp(fileType, ".ppm")) {
		GLuint texture[1];     // storage for one texture

							   // Create ID for texture
		glGenTextures(1, &texture[0]);

		// Set this texture to be the one we are working with
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// Set bi-linear filtering for both minification and magnification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

		int twidth, theight;   // texture width/height [pixels]
		unsigned char* tdata;  // texture pixel data
							   // Load image file
		tdata = loadPPM(textureName, twidth, theight);
		//std::cerr << "Cat Image: " << twidth << " X " << theight << std::endl;
		if (tdata == NULL) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}

		if (texWidth && texHeight) {
			*texWidth = twidth;
			*texHeight = theight;
		}

		// Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture[0];
	}
	else {
		wchar_t filename[512];
		mbstowcs(filename, textureName, 512);

		return wicLoadTexture(filename, texWidth, texHeight, filtering);

		/*std::vector<unsigned char> bytes;
		unsigned width, height;

		if (lodepng::decode(bytes, width, height, textureName)) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}

		if (texWidth && texHeight) {
			*texWidth = width;
			*texHeight = height;
		}

		// Set this texture to be the one we are working with
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(bytes[0]));*/

		/*wchar_t filename[512];
		mbstowcs(filename, textureName, 512);

		if (WICTexImage2DFromFile(texture[0], 0, filename) < 0) {
			glDeleteTextures(1, &texture[0]);
			return -1;
		}*/
	}
}
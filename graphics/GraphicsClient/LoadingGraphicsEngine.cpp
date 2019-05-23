#include "LoadingGraphicsEngine.h"

#include "shader.h"
#include "textureloader.h"

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "loadingscreen_bkg.png")
#define DOT_TEX_PATH (TEXTURE_PATH "loadingscreen_dot.png")
#define PAW_TEX_PATH (TEXTURE_PATH "loadingscreen_paw.png")

static GLuint passthroughShaderProgram = 0;

enum {
	BG_IDX = 0,
	DOT_IDX,
	PAW_IDX
};

static struct {
	const char *path;
	GLuint id;
	int width;
	int height;
} textures[] = {
	{ BG_TEX_PATH },
	{ DOT_TEX_PATH },
	{ PAW_TEX_PATH },
};

static const GLfloat fullscreenVertices[] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f,  1.0f,
	1.0f,  1.0f,
};

static const GLfloat textureVertices[] = {
	/*1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f,  1.0f,
	0.0f,  0.0f,*/
	0.0f, 1.0f, 
	1.0f, 1.0f, 
	0.0f, 0.0f, 
	1.0f, 0.0f,
};

static void deleteTextures()
{
	for (auto &texture : textures) {
		if (texture.id) {
			glDeleteTextures(1, &texture.id);
			texture.id = 0;
		}
	}
}



LoadingGraphicsEngine::LoadingGraphicsEngine()
{

}

LoadingGraphicsEngine::~LoadingGraphicsEngine()
{

}

void LoadingGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{

}

void LoadingGraphicsEngine::CleanUp()
{
	deleteTextures();

	if (passthroughShaderProgram) {
		glDeleteShader(passthroughShaderProgram);
		passthroughShaderProgram = 0;
	}
}

void LoadingGraphicsEngine::MainLoopBegin()
{
	passthroughShaderProgram = LoadShaders("./passthrough.vert", "./passthrough.frag");

	for (auto &texture : textures) {
		texture.id = loadTexture(texture.path, &texture.width, &texture.height);
		if (! texture.id) {
			cerr << "Failed to load " << texture.path << endl;
		}
	}
}

void LoadingGraphicsEngine::MainLoopEnd()
{
	CleanUp();
}

void LoadingGraphicsEngine::ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, windowWidth, windowHeight);
}

void LoadingGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void LoadingGraphicsEngine::CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{

}

void LoadingGraphicsEngine::MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}

void LoadingGraphicsEngine::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{

}

void LoadingGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, windowWidth, windowHeight);

	glUseProgram(passthroughShaderProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[BG_IDX].id);

	/*glUniform1i(uniforms[UNIFORM_VIDEOFRAME], 0);

	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXTUREPOSITON, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXTUREPOSITON);*/

	// Swap buffers
	glfwSwapBuffers(window);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
}


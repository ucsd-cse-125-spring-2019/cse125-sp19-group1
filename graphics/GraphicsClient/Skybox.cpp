#include "Skybox.h"

#include "textureloader.h"
#include "shader.h"

// "Sunny Day" skybox by The Chayed used under license CC-BY 3.0
// https://opengameart.org/content/sky-box-sunny-day
#define TEXTURE_PATH L"../Skybox_SunnyDayByTheChayed/"
#define TEXTURE_FORMAT L".bmp"

#define TEXTURE_FRONT   TEXTURE_PATH  L"Front"   TEXTURE_FORMAT
#define TEXTURE_BACK    TEXTURE_PATH  L"Back"    TEXTURE_FORMAT
#define TEXTURE_LEFT    TEXTURE_PATH  L"Left"    TEXTURE_FORMAT
#define TEXTURE_RIGHT   TEXTURE_PATH  L"Right"   TEXTURE_FORMAT
#define TEXTURE_TOP     TEXTURE_PATH  L"Top"     TEXTURE_FORMAT
#define TEXTURE_BOTTOM  TEXTURE_PATH  L"Bottom"  TEXTURE_FORMAT

static const struct {
	GLenum glFace;
	const wchar_t *filename;
} faces[] = {
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, TEXTURE_RIGHT },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, TEXTURE_LEFT },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, TEXTURE_TOP },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, TEXTURE_BOTTOM },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, TEXTURE_FRONT },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, TEXTURE_BACK },
};

static const float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

Skybox::Skybox()
{
	// skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (const auto &face : faces) {
		WicImage image;

		if (!LoadWicImage(face.filename, &image)) {
			cerr << "Failed to load " << face.filename << endl;
		}

		glTexImage2D(face.glFace, 0, GL_RGBA, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.buffer);

		ReleaseWicImage(&image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	shader = LoadShaders("./skyboxShader.vert", "./skyboxShader.frag");
	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "modelview");
}


Skybox::~Skybox()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	glDeleteProgram(shader);
}


void Skybox::Draw(const glm::mat4 &P, const glm::mat4 &V)
{
	glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);  // so we can draw the skybox last
	glUseProgram(shader);

	auto modelview = glm::mat4(glm::mat3(V));  // remove translation from the view matrix
		
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &(P[0][0]));
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	//glDepthFunc(GL_LESS);
}

#include "UIObject.h"
#include "textureloader.h"

FBXObject * UIObject::sharedAltCanvas = nullptr;
int UIObject::altCanvasRefCount = 0;


UIObject::UIObject(const char * texPath) {
	if (altCanvasRefCount++ <= 0) {
		altCanvasRefCount = 1;
		sharedAltCanvas = new FBXObject(CANVAS_MDL_PATH, nullptr, false);
		sharedAltCanvas->SetDepthTest(false);
	}

	textureID = loadTexture(texPath);
}

UIObject::~UIObject() {
	if (--altCanvasRefCount <= 0) {
		altCanvasRefCount = 0;
		if (sharedAltCanvas) {
			delete sharedAltCanvas;
			sharedAltCanvas = nullptr;
		}
	}

	if (textureID != 0 && textureID != (GLuint)-1) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
}

void UIObject::draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	sharedAltCanvas->Draw(shaderProgram, V, P, model, textureID);
}
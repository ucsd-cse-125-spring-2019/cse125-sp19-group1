
#include "UIObject.h"

UIObject::UIObject(const char * texPath) {
	altCanvas = new FBXObject(CANVAS_MDL_PATH, texPath, false);
	altCanvas->SetDepthTest(false);
}

UIObject::~UIObject() {
	delete(altCanvas);
}

void UIObject::draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	altCanvas->Draw(shaderProgram, V, P, model);
}
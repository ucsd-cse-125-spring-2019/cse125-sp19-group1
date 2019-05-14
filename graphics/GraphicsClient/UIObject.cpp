
#include "UIObject.h"

UIObject::UIObject(const char * texPath) {
	altCanvas = new FBXObject(CANVAS_MDL_PATH, texPath, false);
}

UIObject::~UIObject() {
	delete(altCanvas);
}

void UIObject::draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	altCanvas->Draw(shaderProgram, V, P, model);
}
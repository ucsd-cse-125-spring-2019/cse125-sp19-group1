using namespace std;

#include "Geometry.h"

Geometry::Geometry(FBXObject * fbx, GLint shaderProgram) {
	this->fbx = fbx;
	this->shaderProgram = shaderProgram;
	t = glm::mat4(1.0f);
}

void Geometry::draw(glm::mat4 V, glm::mat4 P) {
	glUseProgram(shaderProgram);
	fbx->Draw(shaderProgram, &(t*V), &P);
}

void Geometry::addChild(Node * child) {

}

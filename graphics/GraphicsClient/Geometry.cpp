using namespace std;

#include "Geometry.h"

Geometry::Geometry(FBXObject * fbx, GLint shaderProgram) {
	this->fbx = fbx;
	this->shaderProgram = shaderProgram;
	t = glm::mat4(1.0f);
}

void Geometry::draw(glm::mat4 V, glm::mat4 P, glm::mat4 model) {
	fbx->Draw(shaderProgram, &(V), &P, (model * t));
}

void Geometry::addChild(Node * child) {

}

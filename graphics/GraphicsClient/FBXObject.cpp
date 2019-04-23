#include "FBXObject.h"

FBXObject::FBXObject(const char * path) {
  // initilize stuff
  parse(path);
  std::cout << "Num meshes: " << meshes.size() << std::endl;
}

FBXObject::~FBXObject()
{
  for (int i = 0; i < meshes.size(); i++) {
    delete(meshes[i]);
  }
}

void FBXObject::draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P)
{
  for (int i = 0; i < meshes.size(); i++) {
    meshes[i]->draw(shaderProgram, V, P);
  }
}

void FBXObject::parse(const char *filepath)
{
  // Populate the face indices, vertices, and normals vectors with the object data:
  loadAssimp(filepath, &meshes);
  loadTexture("caustics.ppm");
}

void FBXObject::update() {
  // This function will handle anything that must continuously occur.
}

void FBXObject::translate(float x, float y, float z) {
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->translate(x, y, z);
	}
}

void FBXObject::rotate(float angle, float x, float y, float z) {
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->rotate(angle, x, y, z);
	}
}

bool FBXObject::within_bounds(float x_min, float x_max, float z_min, float z_max) {
	for (int i = 0; i < meshes.size(); i++) {
		if (!(meshes[i]->within_bounds(x_min, x_max, z_min, z_max))) {
			return false;
		}
		return true;
	}
}

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
}

void FBXObject::update() {
  // This function will handle anything that must continuously occur.
}

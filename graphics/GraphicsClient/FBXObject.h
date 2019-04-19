#ifndef FBXOBJECT_H
#define FBXOBJECT_H

#include "MeshEntry.h"
#include "shader.h"
#include "objloader.h"
#include "textureloader.h"

class FBXObject
{
private:
  std::vector<MeshEntry *> meshes;

public:
  FBXObject(const char * obj_path);
  ~FBXObject();
  void draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P);
  void parse(const char* filepath);
  void update();
};

#endif

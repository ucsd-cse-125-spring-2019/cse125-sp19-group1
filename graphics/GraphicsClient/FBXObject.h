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
  void translate(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
  bool within_bounds(float x_min, float x_max, float z_min, float z_max);
};

#endif

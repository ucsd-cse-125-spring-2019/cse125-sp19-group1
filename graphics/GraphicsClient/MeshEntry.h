//
//  MeshEntry.hpp
//  Graphics Handling
//
//  Created by Athena Hosek on 4/16/19.
//  Copyright © 2019 Athena Hosek. All rights reserved.
//

#ifndef MeshEntry_h
#define MeshEntry_h

#include <stdio.h>

#include "Core.h"
#include "shader.h"

class MeshEntry
{
private:
  glm::vec3 ambient, diffuse, specular;
  float shininess;
  
  // These variables are needed for the shader program
  GLuint VAO, VBO_V, VBO_N, VBO_UV, EBO;
  GLuint uProjection, uModelview, uView;
  GLuint uMaterialD, uMaterialA, uMaterialS, uShine;
  
  glm::mat4 toWorld;
  std::vector<unsigned int> faces;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
  
public:
  MeshEntry();
  ~MeshEntry();
  void Init();
  void SetAmbient(glm::vec3 newAmb);
  void SetDiffuse(glm::vec3 newDiff);
  void SetSpecular(glm::vec3 newSpec);
  void SetShine(float newShine);
  void draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P);
  void printMatrix(glm::mat4 * matrix);
  void update();
  std::vector<unsigned int> * getFaces();
  std::vector<glm::vec3> * getVertices();
  std::vector<glm::vec2> * getUVs();
  std::vector<glm::vec3> * getNormals();
  void translate(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
  bool within_bounds(float x_min, float x_max, float z_min, float z_max);
};

#endif /* MeshEntry_h */

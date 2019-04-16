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
  GLuint VAO, VBO_V, VBO_N, EBO;
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
  void draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P);
  void printMatrix(glm::mat4 * matrix);
  void update();
  std::vector<unsigned int> * getFaces();
  std::vector<glm::vec3> * getVertices();
  std::vector<glm::vec2> * getUVs();
  std::vector<glm::vec3> * getNormals();
};

#endif /* MeshEntry_h */

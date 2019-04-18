//
//  MeshEntry.cpp
//  Graphics Handling
//
//  Created by Athena Hosek on 4/16/19.
//  Copyright © 2019 Athena Hosek. All rights reserved.
//

#include "MeshEntry.h"

// Luma values
glm::vec3 luma_amb = glm::vec3(0.24725f, 0.1995f, 0.0745f);
glm::vec3 luma_diff = glm::vec3(0.75164f, 0.60648f, 0.22648f);
glm::vec3 luma_spec = glm::vec3(0.628281f, 0.555802f, 0.366065f);
float luma_shininess = 52.0f;

MeshEntry::MeshEntry() {
}

void MeshEntry::Init() {
  // initialize stuff
  
  this->toWorld = glm::mat4(1.0f);
  this->ambient = luma_amb;
  this->diffuse = luma_diff;
  this->specular = luma_spec;
  this->shininess = luma_shininess;
  
  // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
  glGenVertexArrays(1, &(this->VAO));
  glGenBuffers(1, &(this->VBO_V));
  glGenBuffers(1, &(this->VBO_N));
  glGenBuffers(1, &(this->EBO));
  
  // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
  // Consider the VAO as a container for all your buffers.
  glBindVertexArray(this->VAO);
  
  // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
  // you want to draw, such as vertices, normals, colors, etc.
  glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_V));
  // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
  // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
  glBufferData(GL_ARRAY_BUFFER, ((this->vertices).size() * (3 * sizeof(GLfloat))), (this->vertices).data(), GL_STATIC_DRAW);
  // Enable the usage of layout location 0 (check the vertex shader to see what this is)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                        3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                        GL_FLOAT, // What type these components are
                        GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                        3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                        (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
  
  glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_N));
  glBufferData(GL_ARRAY_BUFFER, ((this->normals).size() * (3 * sizeof(GLfloat))), (this->normals).data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  
  // We've sent the vertex data over to OpenGL, but there's still something missing.
  // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (this->EBO));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((this->faces).size() * sizeof(GLuint)), &((this->faces)[0]), GL_STATIC_DRAW);
  
  // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // Unbind the VAO now so we don't accidentally tamper with it.
  // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
  glBindVertexArray(0);
}

void MeshEntry::SetAmbient(glm::vec3 newAmb) {
	ambient = glm::vec3(newAmb);
}

void MeshEntry::SetDiffuse(glm::vec3 newDiff) {
	diffuse = glm::vec3(newDiff);
}

void MeshEntry::SetSpecular(glm::vec3 newSpec) {
	specular = glm::vec3(newSpec);
}

void MeshEntry::SetShine(float newShine) {
	shininess = newShine;
}

std::vector<unsigned int> * MeshEntry::getFaces() {
  return &faces;
}

std::vector<glm::vec3> * MeshEntry::getVertices() {
  return &vertices;
}

std::vector<glm::vec2> * MeshEntry::getUVs() {
  return &uvs;
}

std::vector<glm::vec3> * MeshEntry::getNormals() {
  return &normals;
}

MeshEntry::~MeshEntry()
{
  // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
  // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
  glDeleteVertexArrays(1, &(this->VAO));
  glDeleteBuffers(1, &(this->VBO_V));
  glDeleteBuffers(1, &(this->VBO_N));
  glDeleteBuffers(1, &(this->EBO));
}

void MeshEntry::draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P)
{
  // Calculate the combination of the model and view (camera inverse) matrices
  glm::mat4 modelview = (*V) * toWorld;
  
  // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
  // Consequently, we need to forward the projection, view, and model matrices to the shader programs
  // Get the location of the uniform variables "projection" and "modelview"
  uProjection = glGetUniformLocation(shaderProgram, "projection");
  uModelview = glGetUniformLocation(shaderProgram, "modelview");
  uMaterialD = glGetUniformLocation(shaderProgram, "diffColor");
  uMaterialA = glGetUniformLocation(shaderProgram, "ambColor");
  uMaterialS = glGetUniformLocation(shaderProgram, "specColor");
  uShine = glGetUniformLocation(shaderProgram, "shineAmt");
  // Now send these values to the shader program
  glUniformMatrix4fv(uProjection, 1, GL_FALSE, &((*P)[0][0]));
  glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
  glUniform3f(uMaterialD, (this->diffuse)[0], (this->diffuse)[1], (this->diffuse)[2]);
  glUniform3f(uMaterialA, (this->ambient)[0], (this->ambient)[1], (this->ambient)[2]);
  glUniform3f(uMaterialS, (this->specular)[0], (this->specular)[1], (this->specular)[2]);
  glUniform1f(uShine, (this->shininess));
  // Sending the model without the view:
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
  // Now draw the object. We simply need to bind the VAO associated with it.
  glBindVertexArray(this->VAO);
  // Tell OpenGL to draw with triangles, the number of triangles, the type of the indices, and the offset to start from
  glDrawElements(GL_TRIANGLES, (GLsizei) (this->faces).size(), GL_UNSIGNED_INT, 0);
  // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
  glBindVertexArray(0);
}


void MeshEntry::printMatrix(glm::mat4 * matrix) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cerr << (*matrix)[i][j] << " ";
    }
    std::cerr << std::endl;
  }
  std::cerr << std::endl;
}

void MeshEntry::update() {
  // This function will handle anything that must continuously occur.
}

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "Skeleton.h"

// Include Assimp
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>  // Output data structure
#include <assimp/postprocess.h> // Post processing flags

bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * faces, std::vector<glm::vec2> * uvs);
bool loadWithSkeleton(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * faces, std::vector<glm::vec2> * uvs, Skeleton * skel);
void traverseSkeleton(aiNode * currRoot, Skeleton * skel);
void populateSkelVertices(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<Vertex *> * skelVertices);
glm::mat4 * aiMatTOglm(aiMatrix4x4 mat);
void populateMesh(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * faces, std::vector<glm::vec2> * uvs);

#endif

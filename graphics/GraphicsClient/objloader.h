
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "Skeleton.h"
#include "animloader.h"

// Include Assimp
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>  // Output data structure
#include <assimp/postprocess.h> // Post processing flags


#ifndef OBJLOADER_H
#define OBJLOADER_H


struct VertexBoneData
{
	GLuint IDs[4];
	float Weights[4];
	void AddBoneData(GLuint BoneID, float Weight);
};

bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * faces, std::vector<glm::vec2> * uvs, std::vector<VertexBoneData> bones, 
	Skeleton * skel, AnimationPlayer ** animPlayer);
void loadBones(const aiMesh* pMesh, std::vector<VertexBoneData>& Bones, Skeleton * skel);
void loadSkeleton(aiMesh * mesh, aiNode * root, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals, Skeleton * skel);
void traverseSkeleton(aiNode * currRoot, Skeleton * skel);
void assignOffsetMatrices(aiMesh * mesh, Skeleton * skel);
void populateSkelVertices(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals, std::vector<Vertex *> * skelVertices);
glm::mat4 * aiMatTOglm(aiMatrix4x4 mat);
void populateMesh(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * faces, std::vector<glm::vec2> * uvs);

#endif

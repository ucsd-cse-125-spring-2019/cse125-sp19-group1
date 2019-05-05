#include "Skeleton.h"

Skeleton::Skeleton() {
	worldMtx = glm::mat4(1.0f);
}

Skeleton::~Skeleton() {

}

void Skeleton::AddNode(string name, Bone * newNode) {
	if (bones.size() == 0)
		root = newNode;

	bones.insert(std::pair<string, Bone *>(name, newNode));
}

Bone * Skeleton::GetBone(string name) {
	return bones[name];
}

std::map<string, Bone *> * Skeleton::GetBones() {
	return &bones;
}

std::vector<Vertex *> * Skeleton::GetVertices() {
	return &vertices;
}

void Skeleton::PrintBoneStructure() {
	root->Print("");
}

void Skeleton::PrintVertex(unsigned int id) {
	vertices[id]->Print();
}

void Skeleton::Update(glm::mat4 * globalInverseT) {
	glm::mat4 identity = glm::mat4(1.0);
	root->Update(globalInverseT, &identity);
}
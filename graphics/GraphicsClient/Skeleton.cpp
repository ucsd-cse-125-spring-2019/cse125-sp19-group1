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

Bone * Skeleton::GetBone(const string &name) const {
	const auto it = bones.find(name);
	return (it != bones.end()) ? it->second : nullptr;
}

std::unordered_map<string, Bone *> * Skeleton::GetBones() const {
	return (std::unordered_map<string, Bone *> *)&bones;
}

std::vector<Vertex *> * Skeleton::GetVertices() const {
	return (std::vector<Vertex *> *)&vertices;
}

void Skeleton::PrintBoneStructure() {
	root->Print("");
}

void Skeleton::PrintVertex(unsigned int id) {
	vertices[id]->Print();
}

void Skeleton::Update(glm::mat4 globalInverseT) {
	glm::mat4 identity = glm::mat4(1.0);
	root->Update(globalInverseT, identity);
}
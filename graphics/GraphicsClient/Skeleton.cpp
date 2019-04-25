#include "Skeleton.h"

Skeleton::Skeleton() {
	worldMtx = glm::mat4(1.0f);
}

void Skeleton::InitVertices(int numVerts) {

}

std::vector<Bone *> * Skeleton::GetTree() {
	return &tree;
}

std::vector<Vertex *> * Skeleton::GetVertices() {
	return &vertices;
}
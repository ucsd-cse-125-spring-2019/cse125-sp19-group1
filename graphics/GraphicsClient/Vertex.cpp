#include "Vertex.h"

Vertex::Vertex(unsigned int newID, glm::vec3 * newPos, glm::vec3 * newNorm) 
{
	id = newID;
	position = glm::vec3(*newPos);
	normal = glm::vec3(*newNorm);
}

void Vertex::AddWeight(string boneName, float boneWeight) {
	weights.push_back(std::pair<string, float>(boneName, boneWeight));
}

std::vector<std::pair<string, float>> * Vertex::GetWeights() {
	return &weights;
}

glm::vec3 Vertex::GetPos() {
	return position;
}

glm::vec3 Vertex::GetNorm() {
	return normal;
}

unsigned int Vertex::GetID() {
	return id;
}

void Vertex::Print() {
	std::cout << "for vertex (" << position[0] << ", " << position[1] << ", " << position[2] << "): ";
	float total = 0;
	for (int i = 0; i < weights.size(); i++) {
		std::pair<string, float> currWeight = weights[i];
		std::cout << currWeight.first << ", " << currWeight.second << "; ";
		total += currWeight.second;
	}
	std::cout << std::endl;
}
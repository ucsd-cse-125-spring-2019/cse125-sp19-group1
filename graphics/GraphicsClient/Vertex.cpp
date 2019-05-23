#include "Vertex.h"

Vertex::Vertex(unsigned int newID, glm::vec3 * newPos, glm::vec3 * newNorm) 
{
	id = newID;
	position = glm::vec3(*newPos);
	normal = glm::vec3(*newNorm);
}

void Vertex::AddWeight(string boneName, float boneWeight) {
	if (weights.size() < NUM_WEIGHTS)
		weights.push_back(std::pair<string, float>(boneName, boneWeight));
	else {
		int lightestIndex = 0;
		float lightestWeight = weights[0].second;
		for (int index = 1; index < weights.size(); index++) {
			if (weights[index].second < lightestWeight) {
				lightestIndex = index;
				lightestWeight = weights[index].second;
			}
		}
		// replace the lightest weight if this weight is heavier
		if (boneWeight > lightestWeight)
			weights[lightestIndex] = std::pair<string, float>(boneName, boneWeight);
	}
}

void Vertex::NormalizeWeights() {
	// add dummy weights
	while (weights.size() != NUM_WEIGHTS) {
		weights.push_back(std::pair<string, float>("", 0.0f));
	}
	float totalWeight = 0.0f;
	for (int i = 0; i < weights.size(); i++)
		totalWeight += weights[i].second;

	for (int j = 0; j < weights.size(); j++)
		weights[j].second = weights[j].second / totalWeight;
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
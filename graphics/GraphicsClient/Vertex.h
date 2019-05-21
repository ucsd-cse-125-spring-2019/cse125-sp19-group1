#ifndef Vertex_h
#define Vertex_h

#include "Core.h"
#include "Bone.h"

#define NUM_WEIGHTS 4

class Vertex {

public:
	Vertex(unsigned int newID, glm::vec3 * newPos, glm::vec3 * newNorm);
	~Vertex();
	void AddWeight(string boneName, float boneWeight);
	void NormalizeWeights();
	std::vector<std::pair<string, float>> * GetWeights();
	glm::vec3 GetPos();
	glm::vec3 GetNorm();
	unsigned int GetID();
	void Print();
private:
	unsigned int id;
	glm::vec3 position;
	glm::vec3 normal;
	// string == name of bone, float == weight of bone
	std::vector<std::pair<string, float>> weights;
};

#endif
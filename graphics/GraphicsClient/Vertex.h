#ifndef Vertex_h
#define Vertex_h

#include "Core.h"
#include "Bone.h"

class Vertex {

public:
	Vertex(unsigned int newID, glm::vec3 * newPos);
	~Vertex();
	void AddWeight(string boneName, float boneWeight);
	void Print();
private:
	unsigned int id;
	glm::vec3 position;
	// string == name of bone, float == weight of bone
	std::vector<std::pair<string, float>> weights;
};

#endif
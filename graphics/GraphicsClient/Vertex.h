#ifndef Vertex_h
#define Vertex_h

#include "Core.h"
#include "Bone.h"

class Vertex {

public:
	Vertex();
	~Vertex();
	void AddWeight(string boneName, float boneWeight);
private:
	// string == name of bone, float == weight of bone
	std::vector<std::pair<string, float>> weights;
};

#endif
#ifndef Vertex_h
#define Vertex_h

#include "Core.h"
#include "Bone.h"

class Vertex {

	struct Weight {
		string name;
		unsigned int id;
		float weight;
	};

public:
	Vertex();
	~Vertex();
private:
	std::vector<Weight> weights;
};

#endif
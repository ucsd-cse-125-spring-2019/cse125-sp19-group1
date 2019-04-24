#ifndef Bone_h
#define Bone_h

#include "Core.h"

class Bone {
public:
	Bone();
	~Bone();
private:
	std::vector<Bone *> children;
	char * name;
	glm::mat4 offsetMat;
	// unsigned int == vertex ID and float == weight
	std::vector<std::pair<unsigned int, float>> weights;
};

#endif
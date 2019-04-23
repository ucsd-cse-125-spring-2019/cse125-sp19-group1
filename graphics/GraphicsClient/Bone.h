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
};

#endif
#ifndef Bone_h
#define Bone_h

#include "Core.h"

class Bone {
public:
	Bone(string newName, glm::mat4 * newOffsetMatrix);
	~Bone();
private:
	std::vector<Bone *> children;
	string name;
	glm::mat4 offsetMat;
};

#endif
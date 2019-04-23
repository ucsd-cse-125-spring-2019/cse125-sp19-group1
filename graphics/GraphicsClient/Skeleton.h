#ifndef Skeleton_h
#define Skeleton_h

#include "Core.h"
#include "Bone.h"

class Skeleton {
public:
	Skeleton();
	~Skeleton();
private:
	Bone * root;
	std::vector<Bone *> tree;
	glm::mat4 worldMtx;
};

#endif
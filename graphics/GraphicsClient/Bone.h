#ifndef Bone_h
#define Bone_h

#include "Core.h"

class Bone {
public:
	Bone(string newName, glm::mat4 * newOffset, Bone * newParent);
	~Bone();
	void AddChild(Bone * child);
	string GetName();
	void Print(string spaces);
private:
	Bone * parent;
	std::vector<Bone *> children;
	string name;
	glm::mat4 offset;
	unsigned int childCount;
};

#endif
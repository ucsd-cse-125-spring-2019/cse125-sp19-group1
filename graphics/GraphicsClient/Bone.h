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
	void SetOffset(glm::mat4 * offset);
	void SetIBM(glm::mat4 * bindingMatrix);
	glm::mat4 * GetOffset();
	glm::mat4 * GetSkinningMatrix();
private:
	Bone * parent;
	std::vector<Bone *> children;
	string name;
	glm::mat4 offset;
	glm::mat4 inverseBindingMatrix;
	unsigned int childCount;
};

#endif
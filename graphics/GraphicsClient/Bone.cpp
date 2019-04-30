#include "Bone.h"

Bone::Bone(string newName, glm::mat4 * newOffset, Bone * newParent) {
	name = string(newName);
	offset = glm::mat4((*newOffset));
	inverseBindingMatrix = glm::mat4(1.0f);
	parent = newParent;
}

Bone::~Bone() {
	// destructor
}

void Bone::AddChild(Bone * child) {
	children.push_back(child);
}

string Bone::GetName() {
	return name;
}

void Bone::SetOffset(glm::mat4 * newOffset) {
	try {
		offset = glm::mat4(*newOffset);
	}
	catch (exception e) {
		std::cerr << "Bone matrix couldn't be updated\n";
	}
}

void Bone::SetSkinningMatrix() {

}

glm::mat4 * Bone::GetOffset() {
	return &offset;
}

glm::mat4 * Bone::GetSkinningMatrix() {
	glm::mat4 skinningMatrix = glm::mat4(offset * inverseBindingMatrix);
	return &skinningMatrix;
}

void Bone::Print(string spaces) {
	std::cout << spaces << name << std::endl;
	for (int i = 0; i < children.size(); i++) {
		if (children[i])
			children[i]->Print(spaces + " ");
	}
}
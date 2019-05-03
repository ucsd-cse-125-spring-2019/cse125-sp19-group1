#include "Bone.h"

Bone::Bone(string newName, glm::mat4 * nodeMat, Bone * newParent) {
	name = string(newName);
	transform = glm::mat4(1.0f);
	nodeTransform = glm::mat4(*nodeMat);
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

glm::mat4 * Bone::GetOffset() {
	return &offset;
}

glm::mat4 * Bone::GetTransform() {
	return &transform;
}

void Bone::SetOffset(glm::mat4 * newOffset) {
	try {
		offset = glm::mat4(*newOffset);
	}
	catch (exception e) {
		std::cerr << "Bone's inverse binding matrix could not be set" << std::endl;
	}
}

void Bone::SetTransform(glm::mat4 * newTransform) {
	try {
		transform = glm::mat4(*newTransform);
	}
	catch (exception e) {
		std::cerr << "Bone's inverse binding matrix could not be set" << std::endl;
	}
}

void Bone::SetChannel(AnimationChannel * newChannel) {
	channel = newChannel;
}

void Bone::Print(string spaces) {
	std::cout << spaces << name << std::endl;
	for (int i = 0; i < children.size(); i++) {
		if (children[i])
			children[i]->Print(spaces + " ");
	}
}

void Bone::Update(glm::mat4 * globalInverseT, glm::mat4 * parentT) {
	if (channel != NULL) {
		glm::mat4 globalT = (*parentT) * (*(channel->GetTransform()));
		transform = (*globalInverseT) * globalT * offset;
		for (int i = 0; i < children.size(); i++) {
			Update(globalInverseT, &globalT);
		}
	}
}
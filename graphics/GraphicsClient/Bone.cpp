#include "Bone.h"

Bone::Bone(string newName, glm::mat4 nodeMat, Bone * newParent) {
	name = string(newName);
	transform = glm::mat4(1.0f);
	offset = glm::mat4(1.0f);
	nodeTransform = glm::mat4(nodeMat);
	parent = newParent;
	isBone = false;
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

glm::mat4 Bone::GetTransform() {
	return transform;
}

void Bone::SetOffset(glm::mat4 newOffset) {
	try {
		offset = glm::mat4(newOffset);
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

void Bone::Update(glm::mat4 globalInverseT, glm::mat4 parentT) {
	glm::mat4 globalT;

	if (channel != NULL) {
		globalT = parentT * (channel->GetTransform());
		if (isBone) {
			// updating the transform matrix, which the vertices will access when updating skin
			transform = globalInverseT * globalT * offset;
			//transform = globalT * offset;
		}
	}
	else {
		globalT = parentT * nodeTransform;
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->Update(globalInverseT, globalT);
	}
}

void Bone::PrintMatrix(glm::mat4 * matrix) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cerr << (*matrix)[i][j] << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
}

void Bone::SetIsBone(bool input) {
	isBone = input;
}
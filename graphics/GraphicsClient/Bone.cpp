#include "Bone.h"

Bone::Bone(const string &newName, glm::mat4 nodeMat, Bone * newParent) {
	id = -1; // default ID; should be updated later (if all goes correctly)
	name = string(newName);
	transform = glm::mat4(1.0f);
	offset = glm::mat4(0.0f);
	nodeTransform = glm::mat4(nodeMat);
	parent = newParent;
	isBone = false;
	channel = NULL;
}

Bone::~Bone() {
	// destructor
}

void Bone::AddChild(Bone * child) {
	children.push_back(child);
}

const string &Bone::GetName() const {
	return name;
}

const glm::mat4 & Bone::GetOffset() const {
	return offset;
}

const glm::mat4 &Bone::GetTransform() const {
	return transform;
}

void Bone::SetOffset(const glm::mat4 &newOffset) {
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

void Bone::CopyParentChannel() {
	AnimationChannel * parentChannel = parent->GetChannel();
	channel = parentChannel;
}

void Bone::Print(string spaces) {
	std::cout << spaces << name << ": " << isBone << " and " << (channel != NULL) << std::endl;
	for (int i = 0; i < children.size(); i++) {
		if (children[i])
			children[i]->Print(spaces + " ");
	}
}

void Bone::Update(glm::mat4 globalInverseT, glm::mat4 parentT) {
	glm::mat4 globalT;

	if (channel != NULL && channelMatrices.size() > 0) {
		if (channel->GetCurrKeyframe() < channelMatrices.size()) {
			globalT = parentT * channelMatrices[channel->GetCurrKeyframe()];
			// updating the transform matrix, which the vertices will access when updating skin
			if (isBone)
				transform = globalInverseT * globalT * offset;
		}
	}

	else
		globalT = parentT * nodeTransform;

	for (int i = 0; i < children.size(); i++)
		children[i]->Update(globalInverseT, globalT);
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

bool Bone::CheckIsBone() const {
	return isBone;
}

void Bone::SetChannelMatrices(float * values, int numValues) {
	int count = 0;
	while (count < numValues) {
		glm::mat4 currMat = glm::mat4(1.0f);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				currMat[j][i] = values[count];
				count++;
			}
		}
		channelMatrices.push_back(currMat);
	}
}

void Bone::SetID(unsigned int newID) {
	id = newID;
}

int Bone::GetID() const {
	return id;
}

AnimationChannel * Bone::GetChannel() const {
	return channel;
}

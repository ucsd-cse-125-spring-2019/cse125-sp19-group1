#include "Bone.h"

Bone::Bone(string newName, glm::mat4 nodeMat, Bone * newParent) {
	id = -1; // default ID; should be updated later (if all goes correctly)
	name = string(newName);
	// TODO: initializing this matrix to zero affects...the ears and tail?!?!?
	// joint5 has animation data in the file, but it doesn't have an assimp channel?
	// note that we are still attaching channel matrix data to it; it just has a null channel
	// not necessarly problematic because channel has essentially been reduced to a counter
	// we may just want to change keyframes here as we go through the matrix array
	transform = glm::mat4(1.0f);
	// TODO: initializing this to zero is okay?
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

	if (channel != NULL && channelMatrices.size() < 1)
		std::cout << "lol";


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

bool Bone::CheckIsBone() {
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

int Bone::GetID() {
	return id;
}

AnimationChannel * Bone::GetChannel() {
	return channel;
}

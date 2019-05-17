#ifndef Bone_h
#define Bone_h

#include "Core.h"
#include "AnimationChannel.h"

class Bone {
public:
	Bone(string newName, glm::mat4 nodeMat, Bone * newParent);
	~Bone();
	void AddChild(Bone * child);
	string GetName();
	void Print(string spaces);
	glm::mat4 * GetOffset();
	glm::mat4 GetTransform();
	void SetOffset(glm::mat4 newOffset);
	void SetTransform(glm::mat4 * newTransform);
	void SetChannel(AnimationChannel * newChannel);
	void Update(glm::mat4 globalInverseT, glm::mat4 parentT);

	void PrintMatrix(glm::mat4 * matrix);
	void SetIsBone(bool input);

	bool CheckIsBone();
	void SetChannelMatrices(float * values, int numValues);
	void SetID(unsigned int newID);
	unsigned int GetID();

private:
	Bone * parent;
	std::vector<Bone *> children;
	AnimationChannel * channel;
	string name;
	glm::mat4 offset;
	glm::mat4 transform;
	glm::mat4 nodeTransform;
	unsigned int childCount;
	bool isBone;
	std::vector<glm::mat4> channelMatrices;
	int id;
};

#endif
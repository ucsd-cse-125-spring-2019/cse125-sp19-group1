#ifndef BONEINFO_H
#define BONEINFO_H
#include "Core.h"
#include <map>
#include "assimp/scene.h"
class BoneInfo
{
public:
	BoneInfo();
	~BoneInfo();
	std::map<string, int> nameIds;
	std::map<int, glm::mat4> offsetMatrices;
	glm::mat4 offsetArray[100];
	glm::mat4 m_GlobalInverseTransform;
	int numBones;
	aiScene * m_pScene;
	void boneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
	void CalcInterpolatedScaling(glm::mat4 ScalingM, float AnimationTime, const aiNodeAnim * pNodeAnim);
	GLuint FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion RotationQ, float AnimationTime, const aiNodeAnim * pNodeAnim);
	GLuint FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim);
	void CalcInterpolatedPosition(glm::mat4 TranslationM, float AnimationTime, const aiNodeAnim * pNodeAnim);
	GLuint FindTranslation(float AnimationTime, const aiNodeAnim * pNodeAnim);
	void SetMatrixToArray();
private:

};
#endif


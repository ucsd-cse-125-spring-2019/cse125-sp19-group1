#include "BoneInfo.h"

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;


	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}

BoneInfo::BoneInfo()
{
}


BoneInfo::~BoneInfo()
{
}

void BoneInfo::boneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity(1.0);

	float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ?
		m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(numBones);

	for (GLuint i = 0; i < numBones; i++) {
		Transforms[i] = offsetMatrices[i];
	}
}

const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, string nodeName) {
	for (int i = 0; i < pAnimation->mNumChannels; i++) {
		if (pAnimation->mChannels[i] && pAnimation->mChannels[i]->mNodeName == aiString(nodeName)) {
			return pAnimation->mChannels[i];
		}
	}
	return nullptr;
}

void BoneInfo::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation;
	if (m_pScene->mNumAnimations > 1) {
		 pAnimation = m_pScene->mAnimations[1];
	}
	else {
		pAnimation = m_pScene->mAnimations[0];
	}

	glm::mat4 NodeTransformation(aiMatrix4x4ToGlm(&pNode->mTransformation));

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		glm::mat4 ScalingM(1.0);
		CalcInterpolatedScaling(ScalingM, AnimationTime, pNodeAnim);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = aiMatrix4x4ToGlm(&aiMatrix4x4(RotationQ.GetMatrix()));

		// Interpolate translation and generate translation transformation matrix
		glm::mat4 TranslationM(1.0);
		CalcInterpolatedPosition(TranslationM, AnimationTime, pNodeAnim);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (nameIds.find(NodeName) != nameIds.end()) {
		GLuint BoneIndex = nameIds[NodeName];
		offsetMatrices[BoneIndex] = m_GlobalInverseTransform * GlobalTransformation *
			offsetMatrices[BoneIndex];
	}

	for (GLuint i = 0; i < pNode->mNumChildren; i++) {
		if (pNode->mChildren[i]) {
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}
}

void BoneInfo::CalcInterpolatedScaling(glm::mat4 ScalingM, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		aiVector3D scalingVec = pNodeAnim->mScalingKeys[0].mValue;
		ScalingM = glm::scale(glm::mat4(1.0), glm::vec3(scalingVec.x, scalingVec.y, scalingVec.z));
		return;
	}

	GLuint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	aiVector3D scalingVec = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	ScalingM = glm::scale(glm::mat4(1.0), glm::vec3(scalingVec.x, scalingVec.y, scalingVec.z));
	return;
}

GLuint BoneInfo::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
}

void BoneInfo::CalcInterpolatedRotation(aiQuaternion RotationQ, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		RotationQ = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	GLuint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	GLuint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(RotationQ, StartRotationQ, EndRotationQ, Factor);
	RotationQ = RotationQ.Normalize();
}

GLuint BoneInfo::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
}

void BoneInfo::CalcInterpolatedPosition(glm::mat4 TranslationM, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		aiVector3D scalingVec = pNodeAnim->mScalingKeys[0].mValue;
		TranslationM = glm::scale(glm::mat4(1.0), glm::vec3(scalingVec.x, scalingVec.y, scalingVec.z));
		return;
	}

	GLuint PositionIndex = FindTranslation(AnimationTime, pNodeAnim);
	aiVector3D positionVec = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	TranslationM = glm::scale(glm::mat4(1.0), glm::vec3(positionVec.x, positionVec.y, positionVec.z));
	return;
}

GLuint BoneInfo::FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumPositionKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
}

void BoneInfo::SetMatrixToArray() {
	for (int i = 0; i < 100; i++) {
		if (offsetMatrices.find(i) != offsetMatrices.end()) {
			offsetArray[i] = offsetMatrices[i];
		}
	}
}

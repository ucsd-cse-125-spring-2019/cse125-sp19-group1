#include "animloader.h"

bool loadAnimation(aiScene * scene, Skeleton * skel, AnimationPlayer ** animPlayer, int animIndex) {
	// create the scene from which assimp will gather information about the file
	Assimp::Importer importer;

	// exit if the scene fails to load properly
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	/* TODO */
	std::cerr << "Number of animations: " << scene->mNumAnimations << std::endl;
	// be sure to check if animations are there
	// must also determine if we will have multiple animations in a file, but for now, assume 0-1
	if (scene->mNumAnimations < 1) {
		return false;
	}

	aiAnimation * anim;
	if (animIndex > -1 && animIndex < scene->mNumAnimations)
		anim = scene->mAnimations[animIndex];
	else if (scene->mNumAnimations > 1)
		anim = scene->mAnimations[1];
	else
		anim = scene->mAnimations[0];

	if (anim == nullptr || anim->mNumChannels == 0) {
		return false;
	}

	aiMatrix4x4 globalT = scene->mRootNode->mTransformation;
	glm::mat4 globalInverseT = aiMatTOglm_ANIM(globalT.Inverse());

	Animation * newAnimation = new Animation(0, (float)anim->mDuration, globalInverseT);
	std::vector<AnimationChannel*> * channels = newAnimation->GetChannels();

	channels->reserve(anim->mNumChannels);
	convertChannels(anim, channels);

	*animPlayer = new AnimationPlayer(skel, newAnimation);
	return true;
}

void convertChannels(aiAnimation * anim, std::vector<AnimationChannel *> * channels) {

	//assumes there are no keyframes individually holding position/rotation/scaling info
	for (int i = 0; i < anim->mNumChannels; i++) {
		aiNodeAnim * currChannel = anim->mChannels[i];
		//have to map the channels while building them, mold all keyframes together.
		AnimationChannel * newChannel = new AnimationChannel((char*)(currChannel->mNodeName.C_Str()), currChannel->mNumPositionKeys);
		std::vector<Keyframe *> * keyframes = newChannel->getKeyframes();
		channels->push_back(newChannel);
		for (int j = 0; j < currChannel->mNumPositionKeys; j++) {
			aiVectorKey positionKey = currChannel->mPositionKeys[j];
			float keyTime = positionKey.mTime;
			glm::vec3 positionVec = glm::vec3();
			positionVec.x = positionKey.mValue.x;
			positionVec.y = positionKey.mValue.y;
			positionVec.z = positionKey.mValue.z;
			aiQuatKey rotationKey = currChannel->mRotationKeys[j];
			glm::vec4 rotationVec = glm::vec4();
			rotationVec.x = rotationKey.mValue.x;
			rotationVec.y = rotationKey.mValue.y;
			rotationVec.z = rotationKey.mValue.z;
			rotationVec.w = rotationKey.mValue.w;
			aiVectorKey scalingKey = currChannel->mScalingKeys[j];
			glm::vec3 scalingVec = glm::vec3();
			scalingVec.x = scalingKey.mValue.x;
			scalingVec.y = scalingKey.mValue.y;
			scalingVec.z = scalingKey.mValue.z;
			keyframes->push_back(new Keyframe(keyTime, positionVec, rotationVec, scalingVec));
		}
	}
}

// convert aiMatrix4x4 to glm::mat4
// aiMatrix4x4 is row major, while glm is column major
glm::mat4 aiMatTOglm_ANIM(aiMatrix4x4 mat)
{
	glm::mat4 newMat = glm::mat4(1.0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			newMat[i][j] = mat[j][i];
		}
	}

	return newMat;
}

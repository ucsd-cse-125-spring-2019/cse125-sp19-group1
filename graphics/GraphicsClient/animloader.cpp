#include "animloader.h"

bool loadAnimation(aiScene * scene, Skeleton * skel, AnimationPlayer * animPlayer) {
	// create the scene from which assimp will gather information about the file
	Assimp::Importer importer;

	// exit if the scene fails to load properly
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	/* TODO */
	// be sure to check if animations are there
	// must also determine if we will have multiple animations in a file, but for now, assume 0-1
	aiAnimation * anim = scene->mAnimations[0];
	if (!anim || anim->mNumChannels == 0) {
		return false;
	}

	std::cerr << anim->mNumChannels << "\n";
	AnimationChannel ** newChannels = new AnimationChannel*[anim->mNumChannels];
	convertChannels(anim, newChannels);

	Animation * newAnimation = new Animation(anim->mNumChannels, newChannels, 0, (float)anim->mDuration);
	animPlayer = new AnimationPlayer(skel, newAnimation);
	return true;
}

void convertChannels(aiAnimation * anim, AnimationChannel** channels) {

	//assumes there are no keyframes individually holding position/rotation/scaling info
	for (int i = 0; i < anim->mNumChannels; i++) {
		aiNodeAnim * currChannel = anim->mChannels[i];
		//have to map the channels while building them, mold all keyframes together.
			Keyframe ** keyframes = new Keyframe*[currChannel->mNumPositionKeys];
			channels[i] = new AnimationChannel((char*)(currChannel->mNodeName.C_Str()),
				currChannel->mNumPositionKeys, keyframes);
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
				keyframes[j] = new Keyframe(keyTime, positionVec, rotationVec, scalingVec);
				
			}
	}
}
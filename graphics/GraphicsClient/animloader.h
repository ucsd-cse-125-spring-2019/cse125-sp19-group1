#ifndef ANIMLOADER_H
#define ANIMLOADER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <map>

#include <glm/glm.hpp>

// Include Assimp
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>  // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "Skeleton.h"
#include "Animation.h"
#include "AnimationPlayer.h"
#include "AnimationChannel.h"
#include "Keyframe.h"

bool loadAnimation(aiScene * scene, Skeleton * skel, AnimationPlayer ** animPlayer, float animMultiplier, int animIndex);
float convertChannels(aiAnimation * aiAnim, std::vector<AnimationChannel *> * channels, float animMultiplier);
glm::mat4 aiMatTOglm_ANIM(aiMatrix4x4 mat);

#endif
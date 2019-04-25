#include "Bone.h"

Bone::Bone(string newName, glm::mat4 * newOffsetMatrix) {
	name = string(newName);
	offsetMat = glm::mat4((*newOffsetMatrix));
}
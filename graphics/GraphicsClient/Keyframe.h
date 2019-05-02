#ifndef Keyframe_h
#define Keyframe_h
#include "Core.h"
class Keyframe
{
private:
	float time;
	glm::vec3 position;
	glm::vec4 rotation;
	glm::vec3 scaling;
	glm::vec4 coeffs;
public:
	Keyframe(float time, glm::vec3 position, glm::vec4 rotation, glm::vec3 scaling);
	~Keyframe();

	float getTime();
	glm::vec3 getPosition();
	glm::vec4 getRotation();
	glm::vec3 getScaling();
};

#endif


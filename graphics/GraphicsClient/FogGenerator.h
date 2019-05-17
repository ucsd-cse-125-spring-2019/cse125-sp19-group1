#ifndef FogGenerator_h
#define FogGenerator_h
#include "Core.h"
class FogGenerator
{
private:
	float fogDistance;
public:
	FogGenerator(float fogDistance);
	~FogGenerator();
	void setFogDistance(float newFogDistance);
	void draw(GLuint program, glm::vec3 player_pos);
};
#endif


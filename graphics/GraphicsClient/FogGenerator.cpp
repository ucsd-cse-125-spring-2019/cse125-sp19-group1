#include "FogGenerator.h"
FogGenerator::FogGenerator(float fogDistance)
{
	this->fogDistance = fogDistance;
}


FogGenerator::~FogGenerator()
{
}

void FogGenerator::setFogDistance(float newFogDistance) {
	fogDistance = newFogDistance;
}

void FogGenerator::draw(GLuint program, glm::vec3 player_pos) {

	GLuint fog_player_pos, fog_fog_distance;

	fog_player_pos = glGetUniformLocation(program, "fog.player_pos");
	fog_fog_distance = glGetUniformLocation(program, "fog.fog_distance");
	glUniform3f(fog_player_pos, player_pos[0], player_pos[1], player_pos[2]);
	glUniform1f(fog_fog_distance, fogDistance);
}

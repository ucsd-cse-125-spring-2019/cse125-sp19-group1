
#include "Core.h"
#include "textureloader.h"

#ifndef PARTICLE_SPAWNER_H
#define PARTICLE_SPAWNER_H
const int MaxParticles = 1000;
struct Particle {
	glm::vec3 pos, vel;
	glm::vec4 color;
	GLfloat size;
	GLfloat life;
	GLfloat cameraDistance;
};

static const GLfloat g_vertex_buffer_data[] = {
 -0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, 0.5f, 0.0f,
};
class ParticleSpawner
{
public:
	GLfloat g_particle_position_size_data[MaxParticles];
	GLfloat g_particle_color_data[MaxParticles];
	Particle ParticlesContainer[MaxParticles];
	int LastUsedParticle = 0;
	ParticleSpawner(const char *  texPath);
	~ParticleSpawner();
	int FindUnusedParticle();
	void draw(glm::vec3 camPos);

private:
	GLuint textureNum;
	GLuint VAO, billboard_vertex_buffer, particles_position_buffer, particles_color_buffer;
};
#endif


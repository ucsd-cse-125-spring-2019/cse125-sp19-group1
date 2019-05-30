
#include "Core.h"
#include "textureloader.h"


#ifndef PARTICLE_SPAWNER_H
#define PARTICLE_SPAWNER_H

static const GLfloat g_vertex_buffer_data[] = {
	 -0.5f, -0.5f, 0.0f,
	  0.5f, -0.5f, 0.0f,
	 -0.5f,  0.5f, 0.0f,
	  0.5f,  0.5f, 0.0f,
};

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticleSpawner {
public:
	void draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::vec3 CameraPosition, glm::vec3 spawnerPos);
	ParticleSpawner(const char * texPath);
	~ParticleSpawner();
	int FindUnusedParticle();
	void SortParticles();
private:
	double lastTime;
	GLuint texNum;
	GLuint VertexArrayID;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	const int MaxParticles = 100;
	Particle ParticlesContainer[100]; 
	int LastUsedParticle = 0;
	int ParticlesCount = 0;

};

#endif
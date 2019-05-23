#include "ParticleSpawner.h"



ParticleSpawner::ParticleSpawner(const char * texPath)
{
	textureNum = loadTexture(texPath);
	glBindVertexArray(this->VAO);

	LastUsedParticle = 0;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	// see description in function header
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), g_particle_position_size_data, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), g_particle_position_size_data, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_BYTE, GL_FALSE, sizeof(GLubyte), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


ParticleSpawner::~ParticleSpawner()
{
	glDeleteVertexArrays(1, &(this->VAO));
	glDeleteBuffers(1, &(this->billboard_vertex_buffer));
	glDeleteBuffers(1, &(this->particles_position_buffer));
	glDeleteBuffers(1, &(this->particles_color_buffer));
}

int ParticleSpawner::FindUnusedParticle() {

	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void ParticleSpawner::draw(glm::vec3 camPos) {
	// Simulate all particles
	int ParticlesCount = 0;
	glBindTexture(GL_TEXTURE_2D, textureNum);
	for (int i = 0; i < MaxParticles; i++) {

		Particle& p = ParticlesContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= 1;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.vel += glm::vec3(0.0f, 0.3f, 0.0f) * 0.5f;
				p.pos += p.vel;
				p.cameraDistance = glm::length2(p.pos - camPos);
				ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f);

				// Fill the GPU buffer
				g_particle_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particle_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particle_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particle_position_size_data[4 * ParticlesCount + 3] = p.size;

				g_particle_color_data[4 * ParticlesCount + 0] = p.color.x;
				g_particle_color_data[4 * ParticlesCount + 1] = p.color.y;
				g_particle_color_data[4 * ParticlesCount + 2] = p.color.z;
				g_particle_color_data[4 * ParticlesCount + 3] = p.color.w;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameraDistance = -1.0f;
			}

			ParticlesCount++;

		}
	}
	for (int i = 0; i < ParticlesCount; i++) {
		glBindVertexArray(this->VAO);
		// Tell OpenGL to draw with triangles, the number of triangles, the type of the indices, and the offset to start from
		glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(g_vertex_buffer_data)), GL_UNSIGNED_INT, 0);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE, 0);
	}
}

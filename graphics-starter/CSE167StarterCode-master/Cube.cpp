#include "Cube.h"

Cube::Cube(float size)
{
	this->toWorld = glm::mat4(1.0f);
	this->size = size;
	this->angle = 0.0f;
}

Cube::~Cube()
{
}

void Cube::draw()
{
	float halfSize = this->size / 2.0f;

	// Set the OpenGL Matrix mode to ModelView (used when drawing geometry)
	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&toWorld[0][0]);

	// Make cube!
	// Note: The glBegin, and glEnd should always be as close to the glNormal/glVertex calls as possible
	// These are special calls that 'freeze' many internal states of OpenGL.
	// Once the glBegin state is active many of the calls made to OpenGL (like glMultMatrixf) will be IGNORED!
	// As a good habit, only call glBegin just before you need to draw, and call end just after you finish
	glBegin(GL_QUADS);

	// Set the color of the cube (R, G, B). Change it to your favorite color!
	glColor3f(1.0f, 0.95f, 0.1f);

	// Draw front face:
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glVertex3f(halfSize, halfSize, halfSize);
	glVertex3f(halfSize, -halfSize, halfSize);
	glVertex3f(-halfSize, -halfSize, halfSize);

	// Draw left side:
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glVertex3f(-halfSize, halfSize, -halfSize);
	glVertex3f(-halfSize, -halfSize, -halfSize);
	glVertex3f(-halfSize, -halfSize, halfSize);

	// Draw right side:
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(halfSize, halfSize, halfSize);
	glVertex3f(halfSize, halfSize, -halfSize);
	glVertex3f(halfSize, -halfSize, -halfSize);
	glVertex3f(halfSize, -halfSize, halfSize);

	// Draw back face:
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-halfSize, halfSize, -halfSize);
	glVertex3f(halfSize, halfSize, -halfSize);
	glVertex3f(halfSize, -halfSize, -halfSize);
	glVertex3f(-halfSize, -halfSize, -halfSize);

	// Draw top side:
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glVertex3f(halfSize, halfSize, halfSize);
	glVertex3f(halfSize, halfSize, -halfSize);
	glVertex3f(-halfSize, halfSize, -halfSize);

	// Draw bottom side:
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-halfSize, -halfSize, -halfSize);
	glVertex3f(halfSize, -halfSize, -halfSize);
	glVertex3f(halfSize, -halfSize, halfSize);
	glVertex3f(-halfSize, -halfSize, halfSize);

	glEnd();

	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}

void Cube::update()
{
	spin(1.0f);
}

void Cube::spin(float deg)
{
	this->angle += deg;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}


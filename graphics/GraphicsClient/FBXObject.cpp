#include "FBXObject.h"

FBXObject::FBXObject(const char * path, const char * texPath, bool attachSkel) {
	// initialize variables
	Init(attachSkel);
	// read in the model and its texture from the given files
	Parse(path, texPath);
	// initialize rendering variables
	RenderingSetup();
}

void FBXObject::Init(bool attachSkel) {
	this->toWorld = glm::mat4(1.0f);
	this->ambient = default_amb;
	this->diffuse = default_diff;
	this->specular = default_spec;
	this->shininess = default_shininess;
	skel = NULL;
	animPlayer = NULL;
	if (attachSkel)
		skel = new Skeleton();
}

void FBXObject::Parse(const char *filepath, const char *texFilepath)
{
	// Populate the face indices, vertices, and normals vectors with the object data,
	// and potentially load in a Skeleton (if expecting a Skeleton)
	load(filepath, &vertices, &normals, &indices, &uvs, skel, &animPlayer);
	std::cerr << "Printing animPlayer pointer" << animPlayer << "\n";
	// Load the corresponding model texture
	loadTexture(texFilepath);
}

FBXObject::~FBXObject()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &(this->VAO));
	glDeleteBuffers(1, &(this->VBO_V));
	glDeleteBuffers(1, &(this->VBO_N));
	glDeleteBuffers(1, &(this->EBO));
}

void FBXObject::PrintMatrix(glm::mat4 * matrix) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cerr << (*matrix)[i][j] << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
}

void FBXObject::PrintSkeleton() {
	if (hasSkel)
		skel->PrintBoneStructure();
}

void FBXObject::Update() {
	// This function will handle anything that must continuously occur.
	// right now trying to handle updating the animation through this function.
	if (animPlayer != NULL) {
		animPlayer->play();
	}
}

void FBXObject::MoveTo(float x, float y, float z) {
	(this->toWorld)[3][0] = x;
	(this->toWorld)[3][1] = y;
	(this->toWorld)[3][2] = z;
}

void FBXObject::Translate(float x, float y, float z) {
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	this->toWorld = translationMatrix * (this->toWorld);
}

void FBXObject::Rotate(float angle, float x, float y, float z) {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z));
	this->toWorld = rotationMatrix * (this->toWorld);
}

bool FBXObject::WithinBounds(float x_min, float x_max, float z_min, float z_max) {
	float x = (this->toWorld)[3][0];
	float z = (this->toWorld)[3][2];

	if (x > x_min && x < x_max && z > z_min && z < z_max) {
		return true;
	}

	return false;
}

glm::vec3 FBXObject::GetPosition() {
	float x = (this->toWorld)[3][0];
	float y = (this->toWorld)[3][1];
	float z = (this->toWorld)[3][2];
	return glm::vec3(x, y, z);
}

std::vector<glm::vec3> * FBXObject::GetVertices() {
	return &vertices;
}

std::vector<glm::vec3> * FBXObject::GetNormals() {
	return &normals;
}

std::vector<unsigned int> * FBXObject::GetIndices() {
	return &indices;
}

std::vector<glm::vec2> * FBXObject::GetUVs() {
	return &uvs;
}


void FBXObject::SetAmbient(glm::vec3 newAmb) {
	ambient = glm::vec3(newAmb);
}

void FBXObject::SetDiffuse(glm::vec3 newDiff) {
	diffuse = glm::vec3(newDiff);
}

void FBXObject::SetSpecular(glm::vec3 newSpec) {
	specular = glm::vec3(newSpec);
}

void FBXObject::SetShine(float newShine) {
	shininess = newShine;
}


void FBXObject::Draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P)
{
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = (*V) * toWorld;

	// We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uMaterialD = glGetUniformLocation(shaderProgram, "diffColor");
	uMaterialA = glGetUniformLocation(shaderProgram, "ambColor");
	uMaterialS = glGetUniformLocation(shaderProgram, "specColor");
	uShine = glGetUniformLocation(shaderProgram, "shineAmt");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &((*P)[0][0]));
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniform3f(uMaterialD, (this->diffuse)[0], (this->diffuse)[1], (this->diffuse)[2]);
	glUniform3f(uMaterialA, (this->ambient)[0], (this->ambient)[1], (this->ambient)[2]);
	glUniform3f(uMaterialS, (this->specular)[0], (this->specular)[1], (this->specular)[2]);
	glUniform1f(uShine, (this->shininess));
	// Sending the model without the view:
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
	// Now draw the object. We simply need to bind the VAO associated with it.
	glBindVertexArray(this->VAO);
	// Tell OpenGL to draw with triangles, the number of triangles, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)(this->indices).size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

// initialize all the rendering stuff
void FBXObject::RenderingSetup() {

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO_V));
	glGenBuffers(1, &(this->VBO_N));
	glGenBuffers(1, &(this->EBO));
	glGenBuffers(1, &(this->VBO_UV));

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(this->VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_V));
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, ((this->vertices).size() * (3 * sizeof(GLfloat))), (this->vertices).data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_N));
	glBufferData(GL_ARRAY_BUFFER, ((this->normals).size() * (3 * sizeof(GLfloat))), (this->normals).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_UV);
	glBufferData(GL_ARRAY_BUFFER, ((this->uvs).size() * (2 * sizeof(GLfloat))), (this->uvs).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		2, // This second line tells us how any components there are per vertex. In this case, it's 2 (we have an x, y component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		2 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 2 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.



					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (this->EBO));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((this->indices).size() * sizeof(GLuint)), &((this->indices)[0]), GL_STATIC_DRAW);



	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

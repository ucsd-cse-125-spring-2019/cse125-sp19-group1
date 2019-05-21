#include "FBXObject.h"

FBXObject::FBXObject(const char * obj_path, const char * texPath, bool attachSkel) {
	// initialize variables
	Init(attachSkel);
	// read in the model and its texture from the given files
	Parse(obj_path, texPath);
	// initialize rendering variables
	RenderingSetup();
}

void FBXObject::Init(bool attachSkel) {
	this->toWorld = glm::mat4(1.0f);
	this->ambient = default_amb;
	this->diffuse = default_diff;
	this->specular = default_spec;
	this->shininess = default_shininess;
	this->depthTest = true;
	skel = NULL;
	animPlayer = NULL;
	animTimer = 0.0f;
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
	texNum = loadTexture(texFilepath);
	if (animPlayer != NULL) {
		LoadMatrices(filepath);
		skel->Update(animPlayer->GetGlobalInverseT());
	}
}

FBXObject::~FBXObject()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &(this->VAO));
	glDeleteBuffers(1, &(this->VBO_V));
	glDeleteBuffers(1, &(this->VBO_N));
	glDeleteBuffers(1, &(this->EBO));
	glDeleteBuffers(1, &(this->VBO_UV));
	glDeleteBuffers(1, &(this->VBO_WI));
	glDeleteBuffers(1, &(this->VBO_WV));
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
	animTimer++;
	if (fmod(animTimer, 50.0f) == 0.0f) {
		ToNextKeyframe();
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

void FBXObject::SetDepthTest(bool depthTestEnabled) {
	depthTest = depthTestEnabled;
}


void FBXObject::Draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P, glm::mat4 model)
{
	glUseProgram(shaderProgram);
	if (depthTest) {
		glEnable(GL_DEPTH_TEST);
		// Related to shaders and z value comparisons for the depth buffer
		glDepthFunc(GL_LEQUAL);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = (*V) * model;

	glBindTexture(GL_TEXTURE_2D, texNum);

	// We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uMaterialD = glGetUniformLocation(shaderProgram, "diffColor");
	uMaterialA = glGetUniformLocation(shaderProgram, "ambColor");
	uMaterialS = glGetUniformLocation(shaderProgram, "specColor");
	uShine = glGetUniformLocation(shaderProgram, "shineAmt");
	uIsAnimated = glGetUniformLocation(shaderProgram, "isAnimated");
	uBones = glGetUniformLocation(shaderProgram, "bones");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &((*P)[0][0]));
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniform3f(uMaterialD, (this->diffuse)[0], (this->diffuse)[1], (this->diffuse)[2]);
	glUniform3f(uMaterialA, (this->ambient)[0], (this->ambient)[1], (this->ambient)[2]);
	glUniform3f(uMaterialS, (this->specular)[0], (this->specular)[1], (this->specular)[2]);
	glUniform1f(uShine, (this->shininess));

	/*if (skel != NULL || animPlayer != NULL) {
		std::vector<glm::mat4> boneTransforms;
		std::map<string, Bone *> * skelBones = skel->GetBones();
		for (std::map<string, Bone*>::iterator it = skelBones->begin(); it != skelBones->end(); it++)
			boneTransforms.push_back(it->second->GetTransform());
		glUniform1i(uIsAnimated, 1);
		glUniformMatrix4fv(uBones, boneTransforms.size(), GL_FALSE, &((boneTransforms[0])[0][0]));
	}
	else*/
		glUniform1i(uIsAnimated, 0);

	// Sending the model without the view:
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	// Now draw the object. We simply need to bind the VAO associated with it.
	glBindVertexArray(this->VAO);
	// Tell OpenGL to draw with triangles, the number of triangles, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)(this->indices).size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE, 0);
}

// initialize all the rendering stuff
void FBXObject::RenderingSetup() {

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO_V));
	glGenBuffers(1, &(this->VBO_N));
	glGenBuffers(1, &(this->EBO));
	glGenBuffers(1, &(this->VBO_UV));
	glGenBuffers(1, &(this->VBO_WI));
	glGenBuffers(1, &(this->VBO_WV));

	SetBuffers();
}

void FBXObject::UpdateBuffers() {
	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(this->VAO);
	/* send data about vertices */
	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_V));
	glBufferData(GL_ARRAY_BUFFER, ((this->vertices).size() * (3 * sizeof(GLfloat))), (this->vertices).data(), GL_STATIC_DRAW);
	/* send data about normals */
	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_N));
	glBufferData(GL_ARRAY_BUFFER, ((this->normals).size() * (3 * sizeof(GLfloat))), (this->normals).data(), GL_STATIC_DRAW);
	/* send data about uvs */
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_UV);
	glBufferData(GL_ARRAY_BUFFER, ((this->uvs).size() * (2 * sizeof(GLfloat))), (this->uvs).data(), GL_STATIC_DRAW);
	// tell the shader in what order it should draw the vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (this->EBO));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((this->indices).size() * sizeof(GLuint)), &((this->indices)[0]), GL_STATIC_DRAW);
	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

/* Sending relevant data to the shaders
 * glVertexAttribPointer: 
 * - the first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex 
 *   shader (in this case, it's 0,). Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
 * - the second parameter tells us how any components there are per vertex (in this case, it's 3: x, y, and z)
 * - the third parameter tells us what type these components are
 * - the fourth parameter tells us whether or not the values should be normalized (true for yes and false for no)
 * - the fifth parameter tells us the offset between consecutive indices (3 because each vertex has 3 floats)
 * - the sixth paramter tells us the offset of the vertex's first component (0 because we don't pad vertices)
 */
void FBXObject::SetBuffers() {

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(this->VAO);

	/* send data about vertices */
	// GL_ARRAY_BUFFER is an array containing data relevant to what you want to draw (vertices, normals, colors, etc)
	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_V));
	// populating most recently bound buffer with data starting at the 3rd arg and ending after the 2nd arg number of indices
	glBufferData(GL_ARRAY_BUFFER, ((this->vertices).size() * (3 * sizeof(GLfloat))), (this->vertices).data(), GL_STATIC_DRAW);
	// enable usage of layout location 0 (check vertex shader to see which location you need)
	glEnableVertexAttribArray(0);
	// see description in function header
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	/* send data about normals */
	glBindBuffer(GL_ARRAY_BUFFER, (this->VBO_N));
	glBufferData(GL_ARRAY_BUFFER, ((this->normals).size() * (3 * sizeof(GLfloat))), (this->normals).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	/* send data about uvs */
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_UV);
	glBufferData(GL_ARRAY_BUFFER, ((this->uvs).size() * (2 * sizeof(GLfloat))), (this->uvs).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	/*if (animPlayer != NULL) {
		std::vector<glm::vec4> weightIndices;
		std::vector<glm::vec4> weightValues;
		std::vector<Vertex *> * skelVertices = skel->GetVertices();
		for (int i = 0; i < skelVertices->size(); i++) {
			std::vector<std::pair<int, float>> * currWeights = (*skelVertices)[i]->GetWeights();
			glm::vec4 currIndices = glm::vec4(0, 0, 0, 0);
			glm::vec4 currValues = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			// number of weights is restricted to a maximum of four
			for (int j = 0; j < currWeights->size() && j < 4; j++) {
				std::pair<int, float> currWeight = (*currWeights)[j];
				if (currWeight.first != -1) {
					currIndices[j] = currWeight.first;
					currValues[j] = currWeight.second;
				}
			}
			weightIndices.push_back(currIndices);
			weightValues.push_back(currValues);
		}

		/* send data about vertex weight indices /
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO_WI);
		glBufferData(GL_ARRAY_BUFFER, weightIndices.size() * (4 * sizeof(GLuint)), weightIndices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, 4 * sizeof(GLuint), (GLvoid *)0);

		/* send data about vertex weight values /
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO_WV);
		glBufferData(GL_ARRAY_BUFFER, weightValues.size() * (4 * sizeof(GLfloat)), weightValues.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

	}*/

	// tell the shader in what order it should draw the vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (this->EBO));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((this->indices).size() * sizeof(GLuint)), &((this->indices)[0]), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

void FBXObject::ToNextKeyframe() {
	if (animPlayer != NULL) {
		std::cout << "TO NEXT KEYFRAME at time " << animTimer << std::endl;
		animPlayer->ToNextKeyframe();
		skel->Update(animPlayer->GetGlobalInverseT());
		UpdateSkin();
	}
}

void FBXObject::UpdateSkin() {
	std::vector<Vertex *> * skelVertices = skel->GetVertices();
	for (int i = 0; i < skelVertices->size(); i++) {
		Vertex * currV = (*skelVertices)[i];
		std::vector<std::pair<string, float>> * currWeights = currV->GetWeights();
		glm::mat4 M = glm::mat4(0.0f);
		for (int j = 0; j < currWeights->size(); j++) {
			Bone * currBone = skel->GetBone((*currWeights)[j].first);
			if (currBone != NULL)
				M +=  ((*currWeights)[j].second) * currBone->GetTransform();
		}
		vertices[currV->GetID()] = M * glm::vec4(currV->GetPos(), 1.0f);
	}
	UpdateBuffers();
}

void FBXObject::LoadMatrices(const char * path) {
	int bufsize = 128;
	Tokenizer * token = new Tokenizer();
	token->Open(path);
	if (token->FindToken("<library_animations>")) {
		while (token->FindToken("<animation id=\"")) {
			char * out = new char[bufsize];
			token->GetToken(out);
			string boneName = "";
			// extracting the bone name
			for (int index = 0; out[index] != '-' && index < bufsize; index++)
				boneName = boneName + out[index];
			Bone * currBone = skel->GetBone(boneName);
			if (currBone != NULL) {
				string param = string("id=\"") + boneName + string("-Matrix-animation-output-transform-array\" count=\"");
				if (token->FindToken(param.c_str())) {
					int numValues = token->GetInt();
					// skipping to the end of the line to start reading floats
					if (token->FindToken("\">")) {
						float * values = new float[numValues];
						for (int i = 0; i < numValues; i++)
							values[i] = token->GetFloat();
						currBone->SetChannelMatrices(values, numValues);
					}
					else
						std::cout << "ERROR READING FLOATS FOR " << boneName << std::endl;
				}
			}
		}
	}
	else
		std::cout << "COULD NOT FIND LIBRARY ANIMATIONS" << std::endl;

	token->Close();
}
#include "FBXObject.h"

FBXObject::FBXObject(const char * path, const char * texPath, bool attachSkel, int animIndex, bool setupRendering, GLint filtering) {
	// initialize variables
	Init(attachSkel);
	// read in the model and its texture from the given files
	Parse(path, animIndex);

	this->texPath = texPath;
	this->filtering = filtering;

	if (setupRendering) {
		// initialize rendering variables
		RenderingSetup();
	}
}

void FBXObject::Init(bool attachSkel) {
	this->toWorld = glm::mat4(1.0f);
	this->ambient = default_amb;
	this->diffuse = default_diff;
	this->specular = default_spec;
	this->shininess = default_shininess;
	this->depthTest = true;
	this->renderingIsSetup = false;
	this->shouldAnimate = false;
	skel = NULL;
	animPlayer = NULL;
	animTimer = 0.0f;
	if (attachSkel)
		skel = new Skeleton();
}

void FBXObject::Parse(const char *filepath, int animIndex)
{
	// Populate the face indices, vertices, and normals vectors with the object data,
	// and potentially load in a Skeleton (if expecting a Skeleton)
	load(filepath, &vertices, &normals, &indices, &uvs, skel, &animPlayer, animIndex);
	//std::cerr << "Printing animPlayer pointer" << animPlayer << "\n";
	if (animPlayer != NULL) {
		LoadMatrices(filepath);

		const std::vector<Vertex *> * skelVertices = skel->GetVertices();

		weightIndices.resize(skelVertices->size(), glm::ivec4(0, 0, 0, 0));
		weightValues.resize(skelVertices->size(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

		for (int i = 0; i < skelVertices->size(); i++) {
			const std::vector<std::pair<string, float>> * currWeights = (*skelVertices)[i]->GetWeights();
			glm::ivec4 &currIndices = weightIndices[i];
			glm::vec4 &currValues = weightValues[i];
			// number of weights is restricted to a maximum of four
			for (int j = 0; j < currWeights->size() && j < 4; j++) {
				const std::pair<string, float> currWeight = (*currWeights)[j];
				if (currWeight.first.size() != 0) {
					Bone * currBone = skel->GetBone(currWeight.first);
					if (currBone != NULL) {
						if (currBone->GetID() == -1) std::cout << "ID OF -1 ON NECESSARY BONE: " << currBone->GetName() << std::endl;
						else {
							currIndices[j] = currBone->GetID();
							currValues[j] = currWeight.second;
						}
					}
				}
			}
		}
	}
}

FBXObject::~FBXObject()
{
	if (skel) {
		delete skel;
	}

	if (renderingIsSetup) {
		// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
		// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
		glDeleteVertexArrays(1, &(this->VAO));
		glDeleteBuffers(1, &(this->VBO_V));
		glDeleteBuffers(1, &(this->VBO_N));
		glDeleteBuffers(1, &(this->EBO));
	}
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

void FBXObject::Update(bool moving) {
	if (animPlayer != NULL) {
		// if the model believes it should animate but has actually stopped moving, reset animation
		if (shouldAnimate && !moving)
			animPlayer->Reset();

		shouldAnimate = moving;
		if (shouldAnimate)
			Animate();
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

void FBXObject::GetTextureSize(int &width, int &height) {
	width = texWidth;
	height = texHeight;
}

bool FBXObject::GetIsRenderingSetup() {
	return renderingIsSetup;
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


void FBXObject::Draw(GLuint shaderProgram, const glm::mat4 * V, const glm::mat4 * P, glm::mat4 model)
{
	if (!renderingIsSetup)
		return;

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

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &((*P)[0][0]));
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniform3f(uMaterialD, (this->diffuse)[0], (this->diffuse)[1], (this->diffuse)[2]);
	glUniform3f(uMaterialA, (this->ambient)[0], (this->ambient)[1], (this->ambient)[2]);
	glUniform3f(uMaterialS, (this->specular)[0], (this->specular)[1], (this->specular)[2]);
	glUniform1f(uShine, (this->shininess));

	if (shouldAnimate && (skel != NULL || animPlayer != NULL)) {
		glUniform1i(uIsAnimated, 1);
		std::vector<glm::mat4> boneTransforms;
		std::map<string, Bone *> * skelBones = skel->GetBones();
		for (std::map<string, Bone *>::iterator it = skelBones->begin(); it != skelBones->end(); it++) {
			if (it->second->CheckIsBone()) {
				boneTransforms.push_back(it->second->GetTransform());
				string boneString = string("bones[");
				boneString += std::to_string(it->second->GetID());
				boneString += "]";
				uBones = glGetUniformLocation(shaderProgram, boneString.c_str());
				glUniformMatrix4fv(uBones, 1, GL_FALSE, &(boneTransforms[it->second->GetID()][0][0]));
			}
		}
	}
	else
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

	renderingIsSetup = true;

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO_V));
	glGenBuffers(1, &(this->VBO_N));
	glGenBuffers(1, &(this->EBO));
	glGenBuffers(1, &(this->VBO_UV));
	glGenBuffers(1, &(this->VBO_WI));
	glGenBuffers(1, &(this->VBO_WV));

	SetBuffers();

	// Load the corresponding model texture
	texNum = loadTexture(texPath, &texWidth, &texHeight, filtering);
}

void FBXObject::UpdateBuffers() {
	if (!renderingIsSetup)
		return;

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

	if (animPlayer != NULL) {
		/* send data about vertex weight indices */
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO_WI);
		glBufferData(GL_ARRAY_BUFFER, weightIndices.size() * (4 * sizeof(GLuint)), weightIndices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, 4 * sizeof(GLuint), (GLvoid *)0);

		/* send data about vertex weight values */
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO_WV);
		glBufferData(GL_ARRAY_BUFFER, weightValues.size() * (4 * sizeof(GLfloat)), weightValues.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

		weightIndices.clear();
		weightIndices.shrink_to_fit();

		weightValues.clear();
		weightValues.shrink_to_fit();
	}

	// tell the shader in what order it should draw the vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (this->EBO));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((this->indices).size() * sizeof(GLuint)), &((this->indices)[0]), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

void FBXObject::Animate() {
	animPlayer->play();
	skel->Update(animPlayer->GetGlobalInverseT());
}

void FBXObject::LoadMatrices(const char * path) {
	int bufsize = 128;
	Tokenizer * token = new Tokenizer();
	token->Open(path);
	bool print = false;
	if (path == "../Animations/catSearch.dae")
		print = true;
	if (print) {
		std::cout << "LOADING MATS FROM PATH: " << path << std::endl; 
		skel->PrintBoneStructure();
	}
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
						if (print) std::cout << "ADDING MAT TO " << boneName << std::endl;
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
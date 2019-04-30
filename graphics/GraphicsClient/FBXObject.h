#ifndef FBXOBJECT_H
#define FBXOBJECT_H

#include "Core.h"
#include "Skeleton.h"
#include "shader.h"
#include "objloader.h"
#include "textureloader.h"

class FBXObject
{
private:
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	// These variables are needed for the shader program
	GLuint VAO, VBO_V, VBO_N, VBO_UV, EBO;
	GLuint uProjection, uModelview, uView;
	GLuint uMaterialD, uMaterialA, uMaterialS, uShine;
	GLuint texNum;

	glm::mat4 toWorld;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	Skeleton * skel;
	AnimationPlayer * animPlayer;
	bool hasSkel;

	// Luma values
	glm::vec3 default_amb = glm::vec3(0.24725f, 0.1995f, 0.0745f);
	glm::vec3 default_diff = glm::vec3(0.75164f, 0.60648f, 0.22648f);
	glm::vec3 default_spec = glm::vec3(0.628281f, 0.555802f, 0.366065f);
	float default_shininess = 52.0f;

public:
	// creating. destroying, and debugging
	FBXObject(const char * obj_path, const char * tex_path, bool attachSkel);
	void Init(bool attachSkel);
	void Parse(const char* filepath, const char* texFilepath);
	~FBXObject();
	void PrintMatrix(glm::mat4 * matrix);
	void PrintSkeleton();
	// manipulation
	void Update();
	void UpdateSkin();
	void DeformVertex(Vertex * vertex);
	void MoveTo(float x, float y, float z);
	void Translate(float x, float y, float z);
	void Rotate(float angle, float x, float y, float z);
	bool WithinBounds(float xMin, float xMax, float zMin, float zMax);
	// getters
	glm::vec3 GetPosition();
	std::vector<glm::vec3> * GetVertices();
	std::vector<glm::vec3> * GetNormals();
	std::vector<unsigned int> * GetIndices();
	std::vector<glm::vec2> * GetUVs();
	// setters
	void SetAmbient(glm::vec3 newAmb);
	void SetDiffuse(glm::vec3 newDiff);
	void SetSpecular(glm::vec3 newSpec);
	void SetShine(float newShine);
	// rendering
	void Draw(GLuint shaderProgram, glm::mat4 * V, glm::mat4 * P);
	void RenderingSetup();
	void SetBuffers();
};

#endif

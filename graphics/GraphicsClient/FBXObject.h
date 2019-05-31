#ifndef FBXOBJECT_H
#define FBXOBJECT_H

#include "Core.h"
#include "Skeleton.h"
#include "shader.h"
#include "objloader.h"
#include "textureloader.h"
#include "Tokenizer.h"

class FBXObject
{
private:
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	// These variables are needed for the shader program
	GLuint VAO, VBO_V, VBO_N, VBO_UV, VBO_WI, VBO_WV, EBO;
	GLuint uProjection, uModelview, uView;
	GLuint uMaterialD, uMaterialA, uMaterialS, uShine;
	GLuint texNum;
	GLuint uIsAnimated;
	GLuint uBones;

	GLint filtering;

	glm::mat4 toWorld;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	// Temporaries that are deallocated in RenderingSetup
	std::vector<glm::ivec4> weightIndices;
	std::vector<glm::vec4> weightValues;

	const char * texPath;

	Skeleton * skel;
	AnimationPlayer * animPlayer;
	bool hasSkel;
	bool depthTest;
	bool renderingIsSetup;
	bool shouldAnimate;

	int texWidth;
	int texHeight;

	// Luma values
	glm::vec3 default_amb = glm::vec3(0.08725f, 0.0795f, 0.0245f);
	glm::vec3 default_diff = glm::vec3(0.12164f, 0.10648f, 0.034648f);
	glm::vec3 default_spec = glm::vec3(0.118281f, 0.085802f, 0.066065f);
	float default_shininess =  0.5f;

	float animTimer;

public:
	// creating. destroying, and debugging
	FBXObject(const char * obj_path, const char * tex_path, bool attachSkel, int animIndex = -1, bool setupRendering = true, GLint filtering = GL_NEAREST);
	void Init(bool attachSkel);
	void Parse(const char* filepath, int animIndex);
	~FBXObject();
	void PrintMatrix(glm::mat4 * matrix);
	void PrintSkeleton();
	// manipulation
	void Update(bool move);
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
	void GetTextureSize(int &width, int &height);
	// setters
	void SetAmbient(glm::vec3 newAmb);
	void SetDiffuse(glm::vec3 newDiff);
	void SetSpecular(glm::vec3 newSpec);
	void SetShine(float newShine);
	void SetDepthTest(bool depthTestEnabled);
	// rendering
	void Draw(GLuint shaderProgram, const glm::mat4 * V, const glm::mat4 * P, glm::mat4 model);
	void Draw(GLuint shaderProgram, const glm::mat4 * V, const glm::mat4 * P, glm::mat4 model, GLuint textureOverride);
	void RenderingSetup();
	bool GetIsRenderingSetup();
	void UpdateBuffers();
	void SetBuffers();
	void Animate();
	void LoadMatrices(const char * path);
};

#endif

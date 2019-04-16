#ifndef MESH_H
#define MESH_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

struct Vertex
{
	Vector3f m_pos;
	Vector2f m_tex;
	Vector3f m_normal;

	Vertex() {}

	Vertex(const Vector3f& pos, const Vector2f& tex, const Vector3f& normal)
	{
		m_pos = pos;
		m_tex = tex;
		m_normal = normal;
	}

	void resetPos(Vector3f& pos) {
		m_pos = pos;
	}
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool LoadMesh(const std::string& Filename);
	void Render(glm::mat4 * V, glm::mat4 * P, Vector3f playerPos);

	void spin();
	glm::mat4 toWorld;

private:
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void Clear();

	float angle;

#define INVALID_MATERIAL 0xFFFFFFFF

	struct MeshEntry {
		MeshEntry();
		~MeshEntry();

		bool Init(const std::vector<Vertex>& Vertices,
			const std::vector<unsigned int>& Indices);

		GLuint VB;
		GLuint IB;

		std::vector<Vertex> vertices;
		std::vector<Vertex> origVertices;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures;
};

#endif /* MESH_H */
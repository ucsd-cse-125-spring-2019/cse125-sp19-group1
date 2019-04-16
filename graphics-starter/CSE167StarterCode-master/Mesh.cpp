#include <assert.h>

#include "mesh.h"

Mesh::Mesh() {

}

Mesh::~Mesh() {
	Clear();
}

Mesh::MeshEntry::MeshEntry()
{
	VB = GL_INVALID_VALUE;
	IB = GL_INVALID_VALUE;
	NumIndices = 0;
	MaterialIndex = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
	if (VB != GL_INVALID_VALUE) {
		glDeleteBuffers(1, &VB);
	}

	if (IB != GL_INVALID_VALUE) {
		glDeleteBuffers(1, &IB);
	}
}

bool Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
	const std::vector<unsigned int>& Indices)
{
	NumIndices = Indices.size();
	vertices = Vertices;
	origVertices = Vertices;

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(),
		&Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*NumIndices,
		&Indices[0], GL_STATIC_DRAW);

	return true;
}

void Mesh::Clear()
{
	for (unsigned int i = 0; i < m_Textures.size(); i++) {
		SAFE_DELETE(m_Textures[i]);
	}
}

bool Mesh::LoadMesh(const std::string& Filename)
{
	Clear();
	toWorld = glm::mat4(1.0f);
	angle = 0.0f;
	bool Ret = false;

	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(Filename.c_str(),
		aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs);
	std::cerr << "Has Animations: " << pScene->HasAnimations();
	if (pScene) {
		Ret = InitFromScene(pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	return Ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}

	return InitMaterials(pScene, Filename);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
	m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
			&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Vertex v(Vector3f(pPos->x, pPos->y, pPos->z),
			Vector2f(pTexCoord->x, pTexCoord->y),
			Vector3f(pNormal->x, pNormal->y, pNormal->z));
		Vertices.push_back(v);
	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	m_Entries[Index].Init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path,
				NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = Dir + "/" + Path.data;
				m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

				if (!m_Textures[i]->Load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					Ret = false;
				}
			}
		}

		if (!m_Textures[i]) {
			m_Textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");
			Ret = m_Textures[i]->Load();
		}
	}

	return Ret;
}

void Mesh::Render(const glm::mat4 & V, const glm::mat4 & P, Vector3f playerPos)
{
	glm::mat4 modelview = V * toWorld;

	glPushMatrix();
	glMultMatrixf(&toWorld[0][0]);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		std::vector<Vertex> vertices = m_Entries[i].vertices;

		/*for (int j = 0; j < m_Entries[i].vertices.size(); j++) {
			m_Entries[i].vertices[i].resetPos(m_Entries[i].origVertices[i].m_pos + playerPos);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(),
				&vertices[0], GL_STATIC_DRAW);*/
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);


		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glPopMatrix();
}

void Mesh::spin() {
	this->angle += 0.1f;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}
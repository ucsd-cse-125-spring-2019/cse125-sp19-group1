#include "objloader.h"

bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs) 
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	aiMesh * mesh = scene->mMeshes[0];
	populateMesh(mesh, vertices, normals, indices, uvs);

	return true;
}

bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs, Skeleton * skel)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	aiMesh * mesh = scene->mMeshes[0];
	populateMesh(mesh, vertices, normals, indices, uvs);

	std::vector<Vertex *> * skelVertices = skel->GetVertices();
	//populateSkelVertices(mesh, vertices, skelVertices);

	std::vector<Bone *> * bones = skel->GetTree();
	populateSkelBones(mesh, skelVertices, bones);

	return true;
}

void populateSkelBones(aiMesh * mesh, std::vector<Vertex *> * skelVertices, std::vector<Bone *> * bones)
{
	bones->reserve(mesh->mNumBones);
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		aiBone * bone = mesh->mBones[i];
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			int vertexID = bone->mWeights[j].mVertexId;
			if (vertexID < skelVertices->size()) {
				(*skelVertices)[vertexID]->AddWeight(bone->mName.C_Str(), bone->mWeights[j].mWeight));
			}
			else {
				// create a new vertex, then add the weight to it
				skelVertices->push_back(new Vertex());
			}
		}
		bones->push_back(new Bone(bone->mName.C_Str(), (aiMatTOglm(bone->mOffsetMatrix))));
	}
}

glm::mat4 * aiMatTOglm(aiMatrix4x4 mat)
{
	glm::mat4 newMat = glm::mat4(1.0f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			newMat[i][j] = mat[i][j];
		}
	}

	return &newMat;
}

void populateMesh(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs)
{
	// Fill vertices positions
	vertices->reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D pos = mesh->mVertices[i];
		vertices->push_back(glm::vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	uvs->reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		uvs->push_back(glm::vec2(UVW.x, UVW.y));
	}

	// Fill vertices normals
	normals->reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D n = mesh->mNormals[i];
		normals->push_back(glm::vec3(n.x, n.y, n.z));
	}

	// Fill face indices
	indices->reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		// Assume the model has only triangles.
		indices->push_back(mesh->mFaces[i].mIndices[0]);
		indices->push_back(mesh->mFaces[i].mIndices[1]);
		indices->push_back(mesh->mFaces[i].mIndices[2]);
	}
}

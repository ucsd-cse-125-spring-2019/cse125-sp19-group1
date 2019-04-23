#include "objloader.h"

bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs) 
{
	Assimp::Importer importer;

	//const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);// | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	aiMesh * mesh = scene->mMeshes[0];
	populateMesh(mesh, vertices, normals, indices, uvs);
}

bool loadMeshEntries(const char * path, std::vector<MeshEntry *> * meshes) {

	std::vector<unsigned int> * indices;
	std::vector<glm::vec3> * vertices;
	std::vector<glm::vec2> * uvs;
	std::vector<glm::vec3> * normals;

	Assimp::Importer importer;

	//const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);// | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	(*meshes).reserve(scene->mNumMeshes);

	aiMesh * mesh;

	for (unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {

		mesh = scene->mMeshes[mesh_index];

		(*meshes).push_back(new MeshEntry());
		indices = (*meshes)[mesh_index]->getIndices();
		vertices = (*meshes)[mesh_index]->getVertices();
		uvs = (*meshes)[mesh_index]->getUVs();
		normals = (*meshes)[mesh_index]->getNormals();
		if (mesh_index < scene->mNumMaterials) {
			aiMaterial * mat = (scene->mMaterials)[mesh_index];
			glm::vec3 color;
			std::cerr << "Num Textures = " << mat->GetTextureCount(aiTextureType_AMBIENT);
			mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
			(*meshes)[mesh_index]->SetAmbient(color);
			std::cerr << "Ambient Color : " << color.x;
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			(*meshes)[mesh_index]->SetDiffuse(color);
			mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
			(*meshes)[mesh_index]->SetSpecular(color);
			float shininess;
			mat->Get(AI_MATKEY_SHININESS, shininess);
			(*meshes)[mesh_index]->SetShine(shininess);
		}

		populateMesh(mesh, vertices, normals, indices, uvs);
		(*meshes)[mesh_index]->Init();
	}

	// The "scene" pointer will be deleted automatically by "importer"
	return true;
}

void populateMesh(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs)
{
	// Fill vertices positions
	(*vertices).reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D pos = mesh->mVertices[i];
		(*vertices).push_back(glm::vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	(*uvs).reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		(*uvs).push_back(glm::vec2(UVW.x, UVW.y));
	}

	// Fill vertices normals
	(*normals).reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D n = mesh->mNormals[i];
		(*normals).push_back(glm::vec3(n.x, n.y, n.z));
	}

	// Fill face indices
	(*indices).reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		// Assume the model has only triangles.
		(*indices).push_back(mesh->mFaces[i].mIndices[0]);
		(*indices).push_back(mesh->mFaces[i].mIndices[1]);
		(*indices).push_back(mesh->mFaces[i].mIndices[2]);
	}
}

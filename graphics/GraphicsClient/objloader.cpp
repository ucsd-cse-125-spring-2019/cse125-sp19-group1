#include "objloader.h"

bool loadAssimp(const char * path, std::vector<MeshEntry *> * meshes) {
  
  std::vector<unsigned int> * indices;
  std::vector<glm::vec3> * vertices;
  std::vector<glm::vec2> * uvs;
  std::vector<glm::vec3> * normals;

	Assimp::Importer importer;

	//const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate); // | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
  
	if(!scene) {
		fprintf( stderr, importer.GetErrorString());
		getchar();
		return false;
	}
  
  (*meshes).reserve(scene->mNumMeshes);
  std::cout << "Num meshes in loadAssimp: " << scene->mNumMeshes << std::endl;
  
  const aiMesh* mesh;
  
  for (int mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {
    
    mesh = scene->mMeshes[mesh_index];
    
    (*meshes).push_back(new MeshEntry());
    indices = (*meshes)[mesh_index]->getFaces();
    vertices = (*meshes)[mesh_index]->getVertices();
    uvs = (*meshes)[mesh_index]->getUVs();
    normals = (*meshes)[mesh_index]->getNormals();

    // Fill vertices positions
    (*vertices).reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
      aiVector3D pos = mesh->mVertices[i];
      (*vertices).push_back(glm::vec3(pos.x, pos.y, pos.z));
    }
    
    // Fill vertices texture coordinates
    (*uvs).reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
      aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
      (*uvs).push_back(glm::vec2(UVW.x, UVW.y));
    }
    
    // Fill vertices normals
    (*normals).reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
      aiVector3D n = mesh->mNormals[i];
      (*normals).push_back(glm::vec3(n.x, n.y, n.z));
    }
    
    // Fill face indices
    (*indices).reserve(3*mesh->mNumFaces);
    for (unsigned int i=0; i<mesh->mNumFaces; i++){
      // Assume the model has only triangles.
      (*indices).push_back(mesh->mFaces[i].mIndices[0]);
      (*indices).push_back(mesh->mFaces[i].mIndices[1]);
      (*indices).push_back(mesh->mFaces[i].mIndices[2]);
    }
    
    (*meshes)[mesh_index]->Init();
  }
	
	// The "scene" pointer will be deleted automatically by "importer"
	return true;
}

#include "objloader.h"


inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;


	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}


// load a model (and possibly a Skeleton, if the model is expected to have one)
bool load(const char * path, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals,
	std::vector<unsigned int> * indices, std::vector<glm::vec2> * uvs, std::vector<VertexBoneData> bones, 
	Skeleton * skel, AnimationPlayer ** animPlayer) 
{
	// create the scene from which assimp will gather information about the file
	Assimp::Importer importer;
	aiScene* scene = (aiScene *)importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	// exit if the scene fails to load properly
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	// grab the first mesh (assuming, for the purposes of CSE 125, that we will blend models into one mesh)
	aiMesh * mesh = scene->mMeshes[0];
	populateMesh(mesh, vertices, normals, indices, uvs);


	// if the Skeleton pointer is not null, it means we want a Skeleton, so we must load the proper data
	if (skel) {
		skel->boneInfo->m_pScene = scene;
		aiMatrix4x4 globalT = scene->mRootNode->mTransformation;
		//setup inverse transform.
		glm::mat4 globalInverseT = glm::inverse(aiMatrix4x4ToGlm(&(scene->mRootNode->mTransformation)));
		skel->boneInfo->m_GlobalInverseTransform = globalInverseT;
		bones.resize(vertices->size());
		loadBones(mesh, bones, skel);
		loadSkeleton(mesh, scene->mRootNode, vertices, normals, skel);
		loadAnimation((aiScene*)scene, skel, animPlayer);
		std::cerr << "Trying to get animPlayer pointer:" << *animPlayer << "\n";
	}

	// the scene will be destroyed automatically when we return
	return true;
}

void loadBones(const aiMesh* pMesh, std::vector<VertexBoneData>& Bones, Skeleton * skel) {
	for (GLuint i = 0; i < pMesh->mNumBones; i++) {
		GLuint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (skel->boneInfo->nameIds.find(BoneName) == skel->boneInfo->nameIds.end()) {
			BoneIndex = skel->boneInfo->numBones;
			skel->boneInfo->numBones++;
		}
		else {
			BoneIndex =skel->boneInfo->nameIds[BoneName];
		}

		skel->boneInfo->nameIds[BoneName] = BoneIndex;
		skel->boneInfo->offsetMatrices[BoneIndex] = aiMatrix4x4ToGlm(&pMesh->mBones[i]->mOffsetMatrix);

		for (GLuint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			GLuint VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

void VertexBoneData::AddBoneData(GLuint BoneID, float Weight)
{
	for (GLuint i = 0; i < 4; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	//assert(0);
	//std::cerr << "Extra bones" << "\n";
	return;
}



// load a skeleton
void loadSkeleton(aiMesh * mesh, aiNode * root, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals, Skeleton * skel)
{
	// extracting information about how bones affect vertices through weights
	std::vector<Vertex *> * skelVertices = skel->GetVertices();
	populateSkelVertices(mesh, vertices, normals, skelVertices);
	// creating actual Bone objects and populating the Skeleton
	traverseSkeleton(root, skel);
	assignOffsetMatrices(mesh, skel);
}

void traverseSkeleton(aiNode * currNode, Skeleton * skel)
{
	// get the name of the current node
	string name = currNode->mName.C_Str();
	Bone * parent = NULL;

	// if the node has a parent, find the parent and save a pointer to it
	if (currNode->mParent) {
		string parentName = currNode->mParent->mName.C_Str();
		parent = skel->GetBone(parentName);
	}

	// create a Bone for the node and adding it to the Skeleton
	Bone * bone = new Bone(name, aiMatTOglm(currNode->mTransformation), parent);
	skel->AddNode(name, bone);

	// recurse through the tree to add the children to the Skeleton
	for (int i = 0; i < currNode->mNumChildren; i++) {
		aiNode * child = currNode->mChildren[i];
		traverseSkeleton(child, skel);
		// after we've returned from the recursive call, the bone for the child node
		// should now exist, so we can add it to the current bone's list of children
		bone->AddChild(skel->GetBone(child->mName.C_Str()));
	}
}

void assignOffsetMatrices(aiMesh * mesh, Skeleton * skel) {
	aiBone * currAIBone = NULL;
	Bone * currBone = NULL;
	for (int i = 0; i < mesh->mNumBones; i++) {
		currAIBone = mesh->mBones[i];
		currBone = skel->GetBone(currAIBone->mName.C_Str());
		if (currBone != NULL) {
			currBone->SetOffset(aiMatTOglm(currAIBone->mOffsetMatrix));
		}
		else
			std::cout << "OBJLOADER: MISSING A BONE FOR " << currAIBone->mName.C_Str() << std::endl;

		currBone = NULL;
	}
}

void populateSkelVertices(aiMesh * mesh, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals, std::vector<Vertex *> * skelVertices)
{
	// first, populate the skel Vertex array
	skelVertices->reserve(vertices->size());
	for (int i = 0; i < vertices->size(); i++) {
		skelVertices->push_back(new Vertex(i, &((*vertices)[i]), &((*normals)[i])));
	}

	// next, extract weights from the aiBones
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		aiBone * bone = mesh->mBones[i];
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			aiVertexWeight weight = bone->mWeights[j];
			int vertexID = weight.mVertexId;
			// add the weight to the Vertex that it is supposed to influence
			if (vertexID < skelVertices->size())
				(*skelVertices)[vertexID]->AddWeight(bone->mName.C_Str(), weight.mWeight);
			else
				std::cout << "Error loading the bones of the skeleton!" << std::endl;
		}
	}
}

// convert aiMatrix4x4 to glm::mat4
glm::mat4 * aiMatTOglm(aiMatrix4x4 mat)
{
	glm::mat4 newMat = glm::mat4(1.0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			newMat[i][j] = mat[i][j];
		}
	}

	return &newMat;
}

// extract rendering information for a mesh (vertices, noramls, indices/faces, and uvs)
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

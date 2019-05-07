#include "animloader.h"

bool load(const char * path) {
	// create the scene from which assimp will gather information about the file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	// exit if the scene fails to load properly
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}

	/* TODO */
	// be sure to check if animations are there
	// must also determine if we will have multiple animations in a file, but for now, assume 0-1
	aiAnimation * anim = scene->mAnimations[0];

	return true;
}
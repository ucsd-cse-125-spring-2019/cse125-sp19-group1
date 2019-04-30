#ifndef Skeleton_h
#define Skeleton_h

#include "Core.h"
#include <map>
#include "Bone.h"
#include "Vertex.h"

/* Current Plan:
 * Send a Skeleton pointer to the objloader. Read in the aiBone[] from the aiMesh, and create a Bone for each aiBone. 
 * After populating the Bone tree in the objloader, we must go through all the vertices in FBXObject and create
 * a Vertex for each. We can use the data from the Bone[] in this class to determine the weights to be added
 * to each Vertex. The Vertex [] should be in FBX object or in this class --> we just need some way to get the
 * deformed values (for which we need weights and transforms) into the rendering pipeline.
 */

class Skeleton {
public:
	Skeleton();
	~Skeleton();
	void AddNode(string name, Bone * newNode);
	Bone * GetBone(string name);
	std::map<string, Bone *> * GetBones();
	std::vector<Vertex *> * GetVertices();
	void PrintBoneStructure();
	void PrintVertex(unsigned int id);
private:
	Bone * root;
	std::map<string, Bone *> bones;
	std::vector<Vertex *> vertices;
	glm::mat4 worldMtx;
};

#endif
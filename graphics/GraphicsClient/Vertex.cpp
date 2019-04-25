#include "Vertex.h"

Vertex::Vertex() 
{

}

void Vertex::AddWeight(string boneName, float boneWeight) {
	weights.push_back(std::pair<string, float>(boneName, boneWeight));
}
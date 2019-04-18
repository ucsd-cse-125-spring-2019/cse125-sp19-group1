#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "MeshEntry.h"

// Include Assimp
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>  // Output data structure
#include <assimp/postprocess.h> // Post processing flags

bool loadAssimp(const char * path, std::vector<MeshEntry *> * meshes);

#endif

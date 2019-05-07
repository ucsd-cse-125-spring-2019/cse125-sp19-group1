#ifndef ANIMLOADER_H
#define ANIMLOADER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

// Include Assimp
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>  // Output data structure
#include <assimp/postprocess.h> // Post processing flags

bool load(const char * path);

#endif
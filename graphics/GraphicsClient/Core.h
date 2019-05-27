////////////////////////////////////////
// Core.h
////////////////////////////////////////

// This file just has a bunch of common stuff used by all objects. It mainly just
// includes GL and some other standard headers.

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <time.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define CHEF_NAME_SHORT  "Cheoffrey"
#define CHEF_NAME_LONG   "Chef Cheoffrey"
#define GAME_NAME_SHORT  "Restaurant Run"
#define GAME_NAME_LONG   "Chef Cheoffrey's Restaurant Run"

#define ANIMATIONS_PATH  "../Animations/"
#define MODELS_PATH      "../Models/"
#define TEXTURES_PATH    "../Textures/"

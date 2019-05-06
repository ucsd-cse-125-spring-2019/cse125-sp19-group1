#ifndef DirLight_h
#define DirLight_h

#include "Core.h"

class DirLight {
private:
  bool useNormalShading;
  glm::vec3 color;
public:
  glm::vec3 pos;
  glm::vec3 dir;

  DirLight();
  void toggleNormalShading();
  void draw(GLuint program, glm::vec3 * cam_pos, glm::vec3 cam_look_at);
};

#endif /* DirLight_h */

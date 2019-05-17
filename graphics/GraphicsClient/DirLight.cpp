#include "DirLight.h"

DirLight::DirLight () {
  useNormalShading = false;
  color = glm::vec3(1.0f, 1.0f, 1.0f);
  //dir = glm::vec3(0.0f, 0.0f, -1.0f);
  dir = glm::vec3(0.0f, -1.0f, -2.0f);
  //pos = glm::vec3(1.0f, 0.8f, 0.6f);
  pos = glm::vec3(0.0f, 10.0f, 20.0f);

}

void DirLight::toggleNormalShading() {
  useNormalShading = !(useNormalShading);
}

void DirLight::draw(GLuint program, glm::vec3 * cam_pos, glm::vec3 cam_look_at) {
  GLuint normal_shading, light_color, light_dir, light_pos, light_cam_pos, light_cam_look_at;
  
  normal_shading = glGetUniformLocation(program, "light.normal_shading");
  light_color = glGetUniformLocation(program, "light.light_color");
  light_dir = glGetUniformLocation(program, "light.light_dir");
  light_pos = glGetUniformLocation(program, "light.light_pos");
  light_cam_pos = glGetUniformLocation(program, "light.cam_pos");
  light_cam_look_at = glGetUniformLocation(program, "light.cam_look_at");
  
  glUniform1i(normal_shading, useNormalShading);
  glUniform3f(light_color, color[0], color[1], color[2]);
  glUniform3f(light_dir, dir[0], dir[1], dir[2]);
  glUniform3f(light_pos, pos[0], pos[1], pos[2]);
  glUniform3f(light_cam_pos, (*cam_pos)[0], (*cam_pos)[1], (*cam_pos)[2]);
  glUniform3f(light_cam_look_at, (cam_look_at[0]), (cam_look_at[1]), cam_look_at[2]);
}

#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform vec3 diffColor;
uniform vec3 ambColor;
uniform vec3 specColor;
uniform float shineAmt;
uniform mat4 depthBiasMVP;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float transparency;
out vec3 fragNormal;
out vec3 fragPos;
out mat4 originalModel;
out vec3 diffuse;
out vec3 ambient;
out vec3 specular;
out float shininess;
out vec4 ShadowCoord;

void main()
{
  // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
  gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
  transparency = 1.0f;
  fragNormal = normal;
  fragPos = vec3(model * vec4(position, 1.0f));
  originalModel = model;
  diffuse = diffColor;
  ambient = ambColor;
  specular = specColor;
  shininess = shineAmt;
  ShadowCoord = depthBiasMVP * model * vec4(position, 1.0f);
}

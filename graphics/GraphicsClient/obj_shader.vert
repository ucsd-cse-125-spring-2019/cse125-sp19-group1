#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform vec3 diffColor;
uniform vec3 ambColor;
uniform vec3 specColor;
uniform float shineAmt;
uniform mat4 depthBiasMVP;
uniform mat4 gBones[100];
uniform int hasSkel;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float transparency;
out vec3 fragNormal;
out vec3 fragPos;
out vec3 vecPos;
out vec3 viewPos;
out mat4 originalModel;
out vec3 diffuse;
out vec3 ambient;
out vec3 specular;
out float shininess;
out vec4 ShadowCoord;
out vec2 UV;

void main()
{
  // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
  gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
  mat4 BoneTransform;
  vec4 vecPos4;
  if(hasSkel == 1) {
    BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];
	vec4 PosL = BoneTransform * vec4(position, 1.0);
	gl_Position = projection * modelview * PosL;
	//gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	fragNormal = (BoneTransform * vec4(normal, 0.0)).xyz;
	fragPos = vec3(model * PosL);
	vecPos4 = modelview * PosL;
  }
  else {
	BoneTransform = mat4(1.0);
	fragNormal = normal;
	fragPos = vec3(model * vec4(position, 1.0f));
	vecPos4 = modelview * vec4(position.x, position.y, position.z, 1.0);
  }

  transparency = 1.0f;
  viewPos = vec3(vecPos4);
  vecPos = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
  originalModel = model;
  diffuse = diffColor;
  ambient = ambColor;
  specular = specColor;
  shininess = shineAmt;
  ShadowCoord = depthBiasMVP * model * vec4(position, 1.0f);

  //uvs for texturing
  UV = vertexUV;
}

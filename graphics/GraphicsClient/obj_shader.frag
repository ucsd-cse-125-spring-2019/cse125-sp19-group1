#version 330 core

struct Light {
  bool normal_shading;
  vec3 light_color;
  vec3 light_dir;
  vec3 light_pos;
  vec3 cam_pos;
};

uniform Light light;
uniform bool shadowsOn = false;
uniform sampler2D renderedTexture;

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float transparency;
in float test;
in vec3 fragNormal;
in vec3 fragPos;
in mat4 originalModel;
in vec3 diffuse;
in vec3 ambient;
in vec3 specular;
in float shininess;
in vec4 ShadowCoord;
in vec2 UV;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
  //load in texture from sampler2D
  vec3 surfaceToLight = normalize(light.light_pos.xyz);
  vec4 texColor = texture(renderedTexture, UV);
  vec3 amb = vec3(texColor.x, texColor.y, texColor.z);
  //vec3 amb = ambient;
  // Diffuse shading
  float diffuseCoeff = max(0.0f, dot(normal, surfaceToLight));
  vec3 diff = diffuse * diffuseCoeff;
  // Specular shading
  float specularCoeff = 0.0f;
  if (diffuseCoeff > 0.0f) {
    specularCoeff = pow(max(0.0f, dot(viewDir, reflect(-surfaceToLight, normal))), shininess);
  }
  vec3 spec = specular * specularCoeff;
  //TOON SHADING SHIT
  vec4 tempColor;
  float edge = max(0.0,dot(normal, viewDir));
  float intensity = max(dot(normal, normalize(light.light_pos.xyz)), 0);
  
  if (intensity > 0.50) {
    //tempColor = vec4(1.0,1.0,0.0,1.0) * visibility;
    tempColor = vec4((diffuse * 1.25) + (spec * 1.25), 1.0);
    tempColor = vec4(tempColor.xyz, 1.0);
  }
  else if (intensity > 0.20) {
    //tempColor = vec4(1.0,1.0,0.0,1.0) * visibility;
    tempColor = vec4(diffuse + spec, 1.0);
    tempColor = vec4(tempColor.xyz, 1.0);
  }
  else if (intensity > 0.05) {
    //tempColor = vec4(1.0,0.64,0.0,1.0) * visibility;
    tempColor = vec4((diffuse * 0.75) + (spec * 0.75), 1.0);
    tempColor = vec4(tempColor.xyz, 1.0);
  }
  else {
    //tempColor = vec4(0.2,0.05,0.0,1.0) * visibility;
    tempColor = vec4((diffuse * 0.05) + (spec * 0.05 ), 1.0);
    tempColor = vec4(tempColor.xyz, 1.0);;
  }
  
  //If it's on the edge we want it to add a black outline
  if (edge < 0.4) {
    return amb;
  }
  
  return tempColor.xyz + amb;
}

void main()
{
  vec4 texColor = texture(renderedTexture, UV);
  vec3 normal, res, viewDir;
  float visibility = 1.0;
  float bias = 0.005;
  if (shadowsOn) {
    if ( texture( renderedTexture, ShadowCoord.xy ).x  <  ShadowCoord.z-bias) {
      visibility = 0.5;
    }
  }
  normal = normalize((mat3(transpose(inverse(mat3(originalModel))))) * fragNormal);
  viewDir = normalize(light.cam_pos - fragPos);
  if (light.normal_shading) {
    res = vec3(normal[0], normal[1], normal[2]);
  } else {
    res = CalcDirLight(normal, viewDir);
  }
  // An alpha of 1.0f means it is not transparent.
  //color = vec4(res.xyz * visibility, transparency);
  color = vec4(res.x * visibility, res.y * visibility, res.z * visibility, transparency);
  color = texColor;
}

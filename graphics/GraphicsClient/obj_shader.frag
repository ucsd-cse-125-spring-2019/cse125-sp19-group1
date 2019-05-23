#version 330 core

struct Light {
  bool normal_shading;
  vec3 light_color;
  vec3 light_dir;
  vec3 light_pos;
  vec3 cam_pos;
  vec3 cam_look_at;
};

struct Fog {
  vec3 player_pos;
  float fog_distance;
};

uniform Fog fog;
uniform Light light;
uniform bool shadowsOn = false;
uniform sampler2D renderedTexture;

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float transparency;
in float test;
in vec3 fragNormal;
in vec3 fragPos;
in vec3 vecPos;
in mat4 originalModel;
in vec3 diffuse;
in vec3 ambient;
in vec3 specular;
in float shininess;
in vec4 ShadowCoord;
in vec2 UV;
in vec4 weights;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
  //load in texture from sampler2D
  vec3 surfaceToLight = normalize(light.light_pos.xyz);
  vec4 texColor = texture(renderedTexture, UV);
  vec3 texColorNoAlpha =  vec3(texColor.x, texColor.y, texColor.z);
  vec3 amb = texColorNoAlpha;
  //vec3 amb = ambient;
  // Diffuse shading
  float diffuseCoeff = max(0.0f, dot(normal, surfaceToLight));
  //vec3 diff = diffuse * diffuseCoeff;
  vec3 diff = texColorNoAlpha * diffuseCoeff;
  // Specular shading
  float specularCoeff = 0.0f;
  if (diffuseCoeff > 0.0f) {
    specularCoeff = pow(max(0.0f, dot(viewDir, reflect(-surfaceToLight, normal))), shininess);
  }
  //vec3 spec = specular * specularCoeff;
  vec3 spec = texColorNoAlpha * specularCoeff * specular;
  //Toon shader Hung Edition
  	float intensity;
	vec3 toonColor = amb + diff + spec;
	intensity = dot(normalize(-light.light_dir),normalize(normal));
	float viewAngle = dot(normalize(viewDir), normalize(normal));

	if(abs(viewAngle) < 0.5f) { //outline
		toonColor = vec3(0,0,0);
	}
	else if (intensity > 0.95){//otherwise, intensities
		//toonColor = vec4(1.0,0.5,0.5,1.0);
		toonColor = toonColor * 0.95;
	}
	else if (intensity > 0.5){
		//toonColor = vec4(0.6,0.3,0.3,1.0);
		toonColor = toonColor * 0.80;
	}
	else if (intensity > 0.25){
		//toonColor = vec4(0.4,0.2,0.2,1.0);
		toonColor = toonColor * 0.70;
	}
	else
		//toonColor = vec4(0.2,0.1,0.1,1.0);
		toonColor = toonColor * 0.5;  

  //return tempColor.xyz + amb;
  //return amb + diff + spec;
  return toonColor;
  
}

vec3 CalcFogOfWar(vec3 inputColor) {
	//hide items beyound certain threshold of playerposition
   vec3 distanceFromPlayer = vecPos - fog.player_pos;
   float distance = length(distanceFromPlayer);
   if(abs(distance) > fog.fog_distance) {
      inputColor = inputColor * 0.8;
   }
   else if(abs(distance) > fog.fog_distance - 20) {
	  inputColor = inputColor * 0.9;
   }
   return inputColor;
}



void main()
{
  vec4 texColor = texture(renderedTexture, UV);
  vec3 normal, res, viewDir;
  float visibility = 1.0;
  float bias = 0.005;
  /**if (shadowsOn) {
    if ( texture( renderedTexture, ShadowCoord.xy ).x  <  ShadowCoord.z-bias) {
      visibility = 0.5;
    }
  }**/
  normal = normalize((mat3(transpose(inverse(mat3(originalModel))))) * fragNormal);
  viewDir = normalize(light.cam_look_at - light.cam_pos);
  if (light.normal_shading) {
    res = vec3(normal[0], normal[1], normal[2]);
  } else {
    res = CalcDirLight(normal, viewDir);
  }
  res = CalcFogOfWar(res);
  // An alpha of 1.0f means it is not transparent.
  //color = vec4(res.xyz * visibility, transparency);
  //if(fract(fragPos.x / 20)  < 0.1 && fract(fragPos.z / 20) < 0.1) {
    //color = vec4(0.0,1.0,0.0,1.0);
  //}
 // else {
  color = vec4(res.x * visibility, res.y * visibility, res.z * visibility, transparency);
 // }
  //color = texColor;
}

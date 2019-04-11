#version 450

layout (location = 0) in vec3 position;

uniform mat4 transform;			// full transform, including projection

void main()
{
	vec4 position4 = vec4(position, 1.0f);
	gl_Position = transform * position4;
}
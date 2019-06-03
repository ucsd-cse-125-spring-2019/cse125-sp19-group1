#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position.x, position.y-0.02, 1.0, 1.0);
}
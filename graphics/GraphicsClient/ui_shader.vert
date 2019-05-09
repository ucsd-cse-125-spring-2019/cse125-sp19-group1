#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vertexUV;

out vec2 uv;

void main()
{
    uv = vertexUV;
    gl_Position = vec4(position.x, position.y-1.02, 1.0, 1.0);
}
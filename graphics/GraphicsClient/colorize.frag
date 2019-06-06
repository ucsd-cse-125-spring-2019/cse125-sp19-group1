#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;
uniform float alpha;
uniform vec3 fillColor;

void main()
{   
	float texAlpha = texture(tex, vec2(1.f - uv.y, uv.x)).a;
    color = vec4(fillColor, texAlpha * alpha);
}
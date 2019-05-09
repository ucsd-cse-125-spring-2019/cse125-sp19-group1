#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;

void main()
{    
	//color = vec4(1.0,1.0,1.0,1.0);
    color =  texture(tex, uv * 3);
	color = vec4(0.0,0.0,0.0,0.0);
}  
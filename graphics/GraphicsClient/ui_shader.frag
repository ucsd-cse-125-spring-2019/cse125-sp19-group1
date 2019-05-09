#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;

void main()
{    
    color =  texture(tex, vec2(uv.x * 3, uv.y * 3));
	if(color.x < 0.01 && color.y < 0.01 && color.z < 0.01) {
		color = vec4(color.x, color.y, color.z, 0.0);
	}
}  
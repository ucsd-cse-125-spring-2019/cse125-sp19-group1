#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;
uniform float alpha;

void main()
{   
	vec4 beforeAlpha = texture(tex, vec2(1.f - uv.y, uv.x));
    color = vec4(beforeAlpha.rgb, beforeAlpha.a * alpha);
}  
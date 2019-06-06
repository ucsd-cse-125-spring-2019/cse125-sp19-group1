#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;
uniform float ratioAngry;
uniform float ratioBoost;
uniform int boostVisible;


bool blackToTrans(vec4 color) {
	if(color.x == 0 && color.y == 0 && color.z == 0) {
		return true;
	}
	else {
		return false;
	}
}

void main()
{   


    color =  texture(tex, vec2(uv.x * 3, uv.y * 3));

	
	if(uv.y * 3 > (2.2374 + ((2.846875 - 2.2374) * max(0,(1 - ratioAngry)))) && uv.y * 3 < 2.84875 && uv.x * 3 > 0.92686875 && uv.x * 3 < 0.960625) {
		color = vec4(0.7176,0.3725,0.3725,1);
	}

	if(boostVisible == 1 && uv.x * 3 > (0.8) && uv.x * 3 < (0.916 - max(0, (ratioBoost)) * .116) && 
		uv.y * 3 < 2.7638 && uv.y * 3 > 2.7388) {
		color = vec4(0.14, 0.8, 0.7, 1);	
	}

}  
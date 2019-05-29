#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;
uniform float ratioAngry;


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

	
	if(blackToTrans(color)){
		color = vec4(0,0,0,0); 
	}
	if(uv.y * 3 > (2.3024 + ((2.836875 - 2.3024) * max(0,(1 - ratioAngry)))) && uv.y * 3 < 2.836875 && uv.x * 3 > 0.92736875 && uv.x * 3 < 0.961125) {
		color = vec4(0.7176,0.3725,0.3725,1);
	}

}  
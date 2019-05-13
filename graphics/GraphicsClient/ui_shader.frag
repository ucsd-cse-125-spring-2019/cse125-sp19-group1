#version 330 core
in vec2 uv;
out vec4 color;

uniform sampler2D tex;


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


}  
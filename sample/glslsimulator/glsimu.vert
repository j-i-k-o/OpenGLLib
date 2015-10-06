R"(
#version 130
attribute vec3 position;
varying vec2 vTexCoord;

void main(){
	vTexCoord	= (position + 1.0).xy / 2.0;
	gl_Position	= vec4(position, 1.0);
}
)"

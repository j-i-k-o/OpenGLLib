R"(
#version 130
precision mediump float;

uniform sampler2D texture;
varying vec2      vTexCoord;

void main(){
	gl_FragColor = texture2D(texture, vTexCoord);
	}
)"

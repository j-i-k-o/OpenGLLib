R"(
#version 120

varying vec3 Vertex;
varying mat4 View;

uniform samplerCube textureobj;

void main()
{
	vec4 Color;
	Color = textureCube(textureobj, mat3(transpose(View))*Vertex);
	gl_FragColor = Color;
}
)"

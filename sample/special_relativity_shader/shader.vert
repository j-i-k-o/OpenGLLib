R"(
#version 120

attribute vec3 normal;
attribute vec3 vertex;
attribute vec2 texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;
varying vec3 Vertex;
varying vec2 Texcrd;

varying mat4 Model;
varying mat4 View;
varying mat4 Projection;

uniform float beta;

void main()
{
	Normal = normal;
	Vertex = vertex;
	Texcrd = texcrd;
	Model = model;
	View= view;
	Projection = projection;
	vec4 curr = view * model * vec4(vertex, 1.0);
	curr.z = sqrt(1-beta*beta)*curr.z;
	curr.z = (1/(1-beta*beta))*curr.z - beta*sqrt((1/(1-beta*beta)))*sqrt((1/(1-beta*beta))*curr.z*curr.z+curr.x*curr.x+curr.y*curr.y);
	gl_Position = projection*curr;
}
)"

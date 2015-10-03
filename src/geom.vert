R"(
#version 400 core

in vec3 norm;
in vec3 vertex;
in vec2 texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal_g;
out vec3 Vertex_g;
out vec2 Texcrd_g;

out mat4 Model_g;
out mat4 View_g;
out mat4 Projection_g;

void main()
{
	Normal_g = norm;
	Vertex_g = vertex;
	Texcrd_g = texcrd;
	Model_g = model;
	View_g = view;
	Projection_g = projection;

	gl_Position = projection*view*model*vec4(vertex, 1.0);
}
)"

R"(
#version 400 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in vec3 Normal_g[];

in vec3 Normal_g[];
in vec3 Vertex_g[];
in vec2 Texcrd_g[];

in mat4 Model_g[];
in mat4 View_g[];
in mat4 Projection_g[];

out vec3 Normal;
out vec3 Vertex;
out vec2 Texcrd;

out mat4 Model;
out mat4 View;
out mat4 Projection;

void main()
{
	Normal = Normal_g[0];
	Vertex = Vertex_g[0];
	Texcrd = Texcrd_g[0];
	Model = Model_g[0];
	View = View_g[0];
	Projection = Projection_g[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	Normal = Normal_g[1];
	Vertex = Vertex_g[1];
	Texcrd = Texcrd_g[1];
	Model = Model_g[1];
	View = View_g[1];
	Projection = Projection_g[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	Normal = Normal_g[2];
	Vertex = Vertex_g[2];
	Texcrd = Texcrd_g[2];
	Model = Model_g[2];
	View = View_g[2];
	Projection = Projection_g[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	Normal = Normal_g[0];
	Vertex = Vertex_g[0];
	Texcrd = Texcrd_g[0];
	Model = Model_g[0];
	View = View_g[0];
	Projection = Projection_g[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}
)"

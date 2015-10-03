R"(
#version 400 core
in vec3 Normal;
in vec3 Vertex;
in vec2 Texcrd;

in mat4 Model;
in mat4 View;
in mat4 Projection;

struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

uniform Light light;

struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Attenuation{
	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Attenuation attenuation;

uniform sampler2D textureobj;

out vec4 FragColor;

void main()
{
	//ambient
	vec4 ambient = light.ambient*material.ambient;
	//diffuse
	vec3 N = normalize(mat3(View*Model)*Normal);
	vec3 P = (View*Model*vec4(Vertex, 1.0)).xyz;
	vec3 L = (View*vec4(light.position, 1.0)).xyz;
	float diffuseLighting = max(dot(N, normalize(L-P)), 0);
	vec4 diffuse = light.diffuse*diffuseLighting*material.diffuse;
	//specular
	vec3 H = normalize(normalize(L-P)+normalize(-P));
	float specularLighting = pow(max(dot(H, N),0), material.shininess);
	if(diffuseLighting <= 0.0)
	{
		specularLighting = 0.0;
	}
	vec4 specular = specularLighting*light.specular*material.specular;
	vec4 texcolor = texture2D(textureobj, Texcrd);
	FragColor = (ambient + diffuse + specular)*(1.0/(attenuation.constant+attenuation.linear*length(L-P)+attenuation.quadratic*length(L-P)*length(L-P)));
}
)"

#version 330 core
//texture
uniform samplerCube skybox_tex;

//3d texture coordinate
in vec3 texCoords;

out vec3 color;


void main()
{
	vec3 T = normalize(texCoords);
	color =  texture(skybox_tex, T).rgb;
}

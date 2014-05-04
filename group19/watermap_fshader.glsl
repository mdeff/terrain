#version 330 core

// Texture.
//uniform samplerCube skyboxTex;

uniform mat4 modelview;

// 3D texture coordinates.
in vec3 texCoords;
in vec4 tmp_position_mv;
in vec4 tmp_gl_position;

// First output buffer is pixel color.
layout(location = 0) out vec3 color;

void main() {
	vec3 incident_eye = vec3(normalize(tmp_position_mv));
	vec3 normal = vec3(normalize (tmp_gl_position));

	vec3 reflected = reflect (incident_eye, normal);
	// convert from eye to world space
	reflected = vec3 (inverse (modelview) * vec4 (reflected, 0.0));

	color = vec3 (1.0,1.0,1.0); //texture (cube_texture, reflected);
}
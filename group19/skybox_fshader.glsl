#version 330 core

// Texture.
uniform samplerCube skyboxTex;

// 3D texture coordinates.
in vec3 texCoords;

// First output buffer is pixel color.
layout(location = 0) out vec3 color;


void main() {
    vec3 T = normalize(texCoords);
    color =  texture(skyboxTex, T).rgb;
}

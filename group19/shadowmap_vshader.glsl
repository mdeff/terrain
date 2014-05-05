#version 330 core

uniform mat4 lightMVP;

// Texture 0. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

//layout(location = 0) in vec3 vertexPosition_modelspace; // position
layout(location = 0) in vec2 position; // position

void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = (position + 1.0) / 2.0;
    float height = texture(heightMapTex, UV).r;

    vec3 displaced = vec3(position.xy, height);

    // Vertex in camera space then projection/clip space.
    gl_Position = lightMVP * vec4(displaced, 1.0);

}

#version 330 core

uniform mat4 projection;
uniform mat4 modelview;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec3 position;

// Position (world coordinates) after heightmap displacement.
out vec3 displaced;


void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = vec2((position.xy+1)/2);
    float height = texture2D(heightMapTex, UV).r;

    displaced = vec3(position.xy, height);

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = modelview * vec4(displaced, 1.0);
    gl_Position = projection * position_mv;

}

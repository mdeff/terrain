#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace; // position
uniform mat4 lightMVP;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = vec2((vertexPosition_modelspace.xy+1.0)/2.0);
    float height = texture(heightMapTex, UV).r;

    vec3 displaced = vec3(vertexPosition_modelspace.xy, height);

    // Vertex in camera space then projection/clip space.
    gl_Position = lightMVP * vec4(displaced, 1.0);
}

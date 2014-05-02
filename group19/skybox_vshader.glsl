#version 330 core

uniform mat4 modelview;
uniform mat4 projection;

in vec3 position;
out vec3 texCoords;


void main() {

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = modelview * vec4(position, 1.0);
    gl_Position = projection * position_mv;

    //output the position for 3D texture lookup.
    texCoords = position;

}


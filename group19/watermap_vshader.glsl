#version 330 core

uniform mat4 modelview;
uniform mat4 projection;

in vec3 position;
in vec4 position_mv;
in vec4 gl_position;

out vec3 texCoords;
out vec4 tmp_position_mv;
out vec4 tmp_gl_position;

void main() {

    // Vertex in camera space then projection/clip space.
    tmp_position_mv = modelview * vec4(position, 1.0);
    tmp_gl_position = projection * position_mv;

    //output the position for 3D texture lookup.
    texCoords = position;

}
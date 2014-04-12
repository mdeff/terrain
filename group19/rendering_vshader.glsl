#version 330 core

uniform mat4 projection;
uniform mat4 model_view;

in vec3 position;

void main() {

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = model_view * vec4(position, 1.0);
    gl_Position = projection * position_mv;

}

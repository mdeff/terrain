#version 330 core

uniform mat4 projection;
uniform mat4 model_view;
uniform sampler2D heightmap;

in vec3 position;

void main() {

    vec2 UV = vec2(position.xy/2);
    float height = texture2D(heightmap, UV).r;

    vec4 pos = vec4(position.x, position.y, height, 1.0);

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = model_view * pos;
    gl_Position = projection * position_mv;

}

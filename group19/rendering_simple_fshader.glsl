#version 330 core

// Pixel color for fragment shader.
in vec3 color_f;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


void main() {

    // Fragment color.
    color = color_f;

}

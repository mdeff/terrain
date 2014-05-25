#version 330 core

// Pixel color.
uniform vec3 color;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 fragColor;


void main() {

    // Fragment color.
    fragColor = color;

}

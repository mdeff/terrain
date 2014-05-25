#version 330 core

// First output buffer is pixel color.
layout(location = 0) out vec3 color;


void main() {

    // Camera pictorial drawn in yellow.
    color = vec3(1.0, 1.0, 0.0);

}

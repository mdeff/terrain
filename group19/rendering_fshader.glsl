#version 330 core

// First output buffer is pixel color.
layout(location = 0) out vec3 color;

// Position (world coordinates) after heightmap displacement.
in vec3 displaced;


void main() {

    float green = displaced.z;
    color = vec3(1.0, green, 0.0);

}

#version 330 core

in vec3 position2;

// First output buffer.
// Which is DrawBuffers[1] = GL_COLOR_ATTACHMENT0
// Which is attached to heightmapTexture.
layout(location = 0) out float height;


void main() {

    // gl_FragCoord.x
    // position2.x
    height = 0.5;

}

#version 330 core

// First output buffer.
// Which is GL_DEPTH_ATTACHMENT,
// Which is attached to shadowMapTexture.
layout(location = 0) out float fragmentdepth;

void main() {
    // Z is the distance to the camera in camera space.
    fragmentdepth = gl_FragCoord.z;
}

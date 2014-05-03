#version 330 core

// First output buffer.
// Which is GL_DEPTH_ATTACHMENT,
// Which is attached to shadowMapTexture.
layout(location = 0) out float fragmentdepth;

void main() {
	fragmentdepth = gl_FragCoord.z;
}

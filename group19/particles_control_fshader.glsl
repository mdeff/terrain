#version 330 core

// Particules last position and velocity textures.
uniform sampler1D particlePosTex;
uniform sampler1D particleVelTex;

// Pixel window-relative coordinates. (0,0) is lower-left corner.
// The window is 1 pixel height. Positions are stored along x.
layout(pixel_center_integer) in vec4 gl_FragCoord;

// Output buffer location = index in the "drawBuffers" array.
layout(location = 0) out vec3 particlePos;
layout(location = 1) out vec3 particleVel;


void main() {

    // Update the particle position.
    particlePos = texelFetch(particlePosTex, int(gl_FragCoord.x), 0).rgb;
    particlePos = vec3(0.0, 0.0, 1.0);

}

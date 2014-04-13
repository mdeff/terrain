#version 330 core

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler1D permTableTex;
// Second texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler1D gradVectTex;

in vec3 position2;

// First output buffer.
// Which is DrawBuffers[1] = GL_COLOR_ATTACHMENT0
// Which is attached to heightmapTexture.
layout(location = 0) out float height;


void main() {

    // gl_FragCoord.x
    // position2.x
//    height = 0.5;

    // World (triangle grid) coordinates are [-1,1].
    // Texture (height map) coordinates are [0,1].
    float s = (position2.x+1)/2;
    height = texture(permTableTex, s).r / 255 + 0.5;

}


#include "particles_control.h"
#include "vertices.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

//#include "particles_control_vshader.h"
//#include "particles_control_fshader.h"


ParticlesControl::ParticlesControl(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


GLuint ParticlesControl::init(Vertices* vertices) {

    /// Common initialization.
//    RenderingContext::init(vertices, particles_control_vshader, particles_control_fshader, "vertexPosition2D", -1);

    /// Create and bind the last particles velocities to texture 0.
//    GLuint particleVelTexID = -1;
//    set_texture(0, particleVelTexID, "particleVelTex", GL_TEXTURE_1D);

    /// Create and bind to texture 2 the texture which will contain the
    /// color output (the actual height map) of our shader. No need to be
    /// binded to a texture index, but it is simpler to use the framework.
    GLuint particlesPosTexID;
    glGenTextures(1, &particlesPosTexID);
    glBindTexture(GL_TEXTURE_1D, particlesPosTexID);

    // nParticles = nVertices
    unsigned int nParticles = _width * _height;

    float particlesPos[3*nParticles];
    for(int k=0; k<nParticles; ++k) {
        particlesPos[3*k+0] = 1.0f; // x
        particlesPos[3*k+1] = 2 * float(k) / float(nParticles) - 1; // y
        particlesPos[3*k+2] = 1.0f; // z
    }

    /// Particles initial position, three color components, unclamped 32 bits float.
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, nParticles, 0, GL_RGB, GL_FLOAT, particlesPos);

    /// Filtering technique has to be set, even that texels are fetch
    /// individually by fetchTexel() which bypass any filtering.
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /// Attach the created texture to the first color attachment point.
    /// The texture becomes the fragment shader first output buffer.
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightMapTexID, 0);
//    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
//    glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

    /// Check that our framebuffer is complete.
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//        std::cerr << "Heightmap framebuffer not complete." << std::endl;
//        exit(EXIT_FAILURE);
//    }

    /// Return the heightmap texture ID (for the terrain).
    return particlesPosTexID;

}


void ParticlesControl::draw() const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.

    /// Clear the FBO.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClear(GL_COLOR_BUFFER_BIT);

    /// Render the height map to FBO.
    _vertices->draw();

}


#include "particles_control.h"
#include "vertices.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "particles_control_vshader.h"
#include "particles_control_fshader.h"


ParticlesControl::ParticlesControl(unsigned int nParticles) :
    RenderingContext(nParticles, 1) {
}


void ParticlesControl::init(Vertices* vertices, GLuint particlePosTexID[]) {

    /// Common initialization.
    RenderingContext::init(vertices, particles_control_vshader, particles_control_fshader, "vertexPosition2D", -1);

    /// Number of particles on the side. That is nParticlesSide^3 particles.
    const int nParticlesSide = 10;
    const int nParticles = nParticlesSide*nParticlesSide*nParticlesSide;

    /// Verify that the texture is sufficiently large to hold all particles.
    if(_width * _height < nParticles) {
        std::cerr << "Particles textures are too small to hold all particles." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Initial particles position and velocity : cube of 2x2x5.
    /// Terrain is 2*2 and skybox has height of 5.
    float particlesPos[3*nParticles];
    float particlesVel[3*nParticles];
    for(int k=0; k<nParticles; ++k) {
        particlesPos[3*k+0] = 2.0f * float(k % (nParticlesSide*nParticlesSide) % nParticlesSide) / float(nParticlesSide) - 1.0f;  // x
        particlesPos[3*k+1] = 2.0f * float(k % (nParticlesSide*nParticlesSide) / nParticlesSide) / float(nParticlesSide) - 1.0f;  // y
        particlesPos[3*k+2] = 5.0f * float(k / (nParticlesSide*nParticlesSide)                 ) / float(nParticlesSide) + 1.0f;  // z
        particlesVel[3*k+0] = 0.0f;  // x
        particlesVel[3*k+1] = 0.0f;  // y
        particlesVel[3*k+2] = 0.0f;  // z
    }

    /// The Sampler uniforms always refer to textures indices 0 and 1.
    /// The binding to textures 0 and 1 are however flipped every frame.
    GLuint uniformID = glGetUniformLocation(_programID, "particlePosTex");
    glUniform1i(uniformID, 0);
    uniformID = glGetUniformLocation(_programID, "particleVelTex");
    glUniform1i(uniformID, 1);

    /// Generate the two position textures : last and current.
    glGenTextures(4, _particleTexID);
    particlePosTexID[0] = _particleTexID[0];
    particlePosTexID[1] = _particleTexID[1];

    /// Position and velocity : three components, unclamped 32 bits float.
    /// Filtering technique has to be set, even that texels are fetch
    /// individually by fetchTexel() which bypass any filtering.
    /// Attach the textures to the corresponding FBO color attachments.
    for(int k=0; k<4; ++k) {
        glBindTexture(GL_TEXTURE_1D, _particleTexID[k]);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _width, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + k, _particleTexID[k], 0);
    }

    /// Initial particules position and velocity.
    glBindTexture(GL_TEXTURE_1D, _particleTexID[1]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, nParticles, 0, GL_RGB, GL_FLOAT, particlesPos);
    glBindTexture(GL_TEXTURE_1D, _particleTexID[3]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, nParticles, 0, GL_RGB, GL_FLOAT, particlesVel);




    /// Attach the created texture to the first color attachment point.
    /// The texture becomes the fragment shader first output buffer.
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _particleTexID[0], 0);
//    glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_1D, _particlePosTexID[0], 0);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Particles framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

}


void ParticlesControl::draw() const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.

    /// Flip the position texture binding.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, _particleTexID[1]);
//    GLuint tmp = _particlePosTexID[0];
//    _particlePosTexID[0] = _particlePosTexID[1];
//    _particlePosTexID[1] = tmp;



    /// Clear the FBO.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClear(GL_COLOR_BUFFER_BIT);

    /// Render the height map to FBO.
    _vertices->draw();

}

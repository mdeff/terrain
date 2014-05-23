
#include "particles_control.h"
#include "vertices.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "particles_control_vshader.h"
#include "particles_control_fshader.h"


/// Texture is 1D, image should thus have an height of 1 pixel.
ParticlesControl::ParticlesControl(unsigned int nParticlesSide) :
    RenderingContext(nParticlesSide*nParticlesSide*nParticlesSide, 1),
    _nParticlesSide(nParticlesSide) {
}


void ParticlesControl::init(Vertices* vertices, GLuint particlePosTexID[]) {

    /// Common initialization.
    RenderingContext::init(vertices, particles_control_vshader, particles_control_fshader, "vertexPosition2D", -1);

    /// The Sampler uniforms always refer to texture indices 0 and 1.
    /// The binding to textures 0 and 1 are however flipped every frame.
    GLuint uniformID = glGetUniformLocation(_programID, "particlePosTex");
    glUniform1i(uniformID, 0);
    uniformID = glGetUniformLocation(_programID, "particleVelTex");
    glUniform1i(uniformID, 1);

    /// Generate the two position and velocity textures (last and current).
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

    /// Initial particles position and velocity.
    /// Particles can be in x=[-1,1], y=[-1,1], z=[0,5].
    unsigned int nParticles = _nParticlesSide*_nParticlesSide*_nParticlesSide;
    float particlesPos[3*nParticles];
    float particlesVel[3*nParticles];
    for(int k=0; k<nParticles; ++k) {
        particlesPos[3*k+0] = 2.0f * float(k % (_nParticlesSide*_nParticlesSide) % _nParticlesSide) / float(_nParticlesSide) - 1.0f;  // x
        particlesPos[3*k+1] = 2.0f * float(k % (_nParticlesSide*_nParticlesSide) / _nParticlesSide) / float(_nParticlesSide) - 1.0f;  // y
        particlesPos[3*k+2] = 2.0f * float(k / (_nParticlesSide*_nParticlesSide)                  ) / float(_nParticlesSide) + 6.2f;  // z
        particlesVel[3*k+0] = 0.0f;  // x
        particlesVel[3*k+1] = 0.0f;  // y
        particlesVel[3*k+2] = 0.1f * float(k / (_nParticlesSide*_nParticlesSide)                  ) / float(_nParticlesSide) + 0.0f;  // z
    }
    glBindTexture(GL_TEXTURE_1D, _particleTexID[0]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _width, 0, GL_RGB, GL_FLOAT, particlesPos);
    glBindTexture(GL_TEXTURE_1D, _particleTexID[2]);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _width, 0, GL_RGB, GL_FLOAT, particlesVel);

    /// Set uniform IDs.
    _deltaTID = glGetUniformLocation(_programID, "deltaT");

}


void ParticlesControl::draw() const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime);
    lastTime = currentTime;
    glUniformMatrix4fv(_deltaTID, 1, GL_FALSE, &deltaT);

    /// Binary [0,1] variable to switch between input / output textures : start with 0.
    static int pingpong = 1;
    pingpong = (pingpong+1) % 2;

    /// Flip the position and velocity texture bindings.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, _particleTexID[pingpong]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, _particleTexID[pingpong+2]);

    /// Flip the position and velocity output buffers attachement bindings.
    const GLenum drawBuffers[][2] = {{GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3},
                                     {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2}};
    glDrawBuffers(2, drawBuffers[pingpong]);

    /// Clear the FBO.
    glClear(GL_COLOR_BUFFER_BIT);

    /// Render the height map to FBO.
    _vertices->draw();

}

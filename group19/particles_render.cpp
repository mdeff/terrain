
#include "particles_render.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "particles_render_vshader.h"
#include "particles_render_fshader.h"


ParticlesRender::ParticlesRender(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void ParticlesRender::init(GLuint particlePosTexID[]) {

    /// Common initialization.
    RenderingContext::init(NULL, particles_render_vshader, particles_render_fshader, "", 0);

    /// Store the position texture IDs.
    _particlePosTexID[0] = particlePosTexID[0];
    _particlePosTexID[1] = particlePosTexID[1];

    /// The Sampler uniform always refer to texture index 0.
    /// The binding to texture 0 is however flipped every frame.
    GLuint uniformID = glGetUniformLocation(_programID, "particlesPosTex");
    glUniform1i(uniformID, 0);

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void ParticlesRender::draw(const mat4& projection, const mat4& modelview) {

    /// Common drawing. 
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Flip the position texture binding.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, _particlePosTexID[0]);
    GLuint tmp = _particlePosTexID[0];
    _particlePosTexID[0] = _particlePosTexID[1];
    _particlePosTexID[1] = tmp;

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the particles from camera point of view to default framebuffer.
    const int nParticlesSide = 10;
    const int nVertices = nParticlesSide*nParticlesSide*nParticlesSide;
    glDrawArrays(GL_POINTS, 0, nVertices);

}

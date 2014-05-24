
#include "particles_render.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "particles_render_vshader.h"
#include "particles_render_fshader.h"


ParticlesRender::ParticlesRender(unsigned int width, unsigned int height, unsigned int nParticlesSide) :
    RenderingContext(width, height),
    _nParticlesSide(nParticlesSide) {
}


void ParticlesRender::init(GLuint particlePosTexID[]) {

    /// Common initialization.
    RenderingContext::init(NULL, particles_render_vshader, particles_render_fshader, "", 0);

    /// Allow programmable point size for the vertex shader to size the sprite.
    glEnable(GL_PROGRAM_POINT_SIZE);

    /// Store the position texture IDs.
    _particlePosTexID[0] = particlePosTexID[0];
    _particlePosTexID[1] = particlePosTexID[1];

    /// The Sampler uniform always refer to texture index 0.
    /// The binding to texture 0 is however flipped every frame.
    GLuint uniformID = glGetUniformLocation(_programID, "particlesPosTex");
    glProgramUniform1i(_programID, uniformID, 0);

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void ParticlesRender::draw(const mat4& projection, const mat4& view) {

    /// Common drawing. 
    RenderingContext::draw();

    /// Blending for particle transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /// Update the content of the uniforms.
    glProgramUniformMatrix4fv(_programID, _viewID, 1, GL_FALSE, view.data());
    glProgramUniformMatrix4fv(_programID, _projectionID, 1, GL_FALSE, projection.data());

    /// Flip the position texture binding : start with 1.
    static int pingpong = 0;
    pingpong = (pingpong+1) % 2;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, _particlePosTexID[pingpong]);

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the particles from camera point of view to default framebuffer.
    unsigned int nVertices = _nParticlesSide*_nParticlesSide*_nParticlesSide;
    glDrawArrays(GL_POINTS, 0, nVertices);

    /// Disable blending : all the other primitives are opaque.
    glDisable(GL_BLEND);

}

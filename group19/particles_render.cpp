
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


void ParticlesRender::init(Vertices* vertices, GLuint particlesPosTexID) {

    /// Common initialization.
    RenderingContext::init(vertices, particles_render_vshader, particles_render_fshader, "particlesPosTexIdx");

    /// Bind the particles positions to texture 0.
    set_texture(0, particlesPosTexID, "particlesPosTex", GL_TEXTURE_1D);

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void ParticlesRender::draw(const mat4& projection, const mat4& modelview) const {

    /// Common drawing. 
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the particles from camera point of view to default framebuffer.
    _vertices->draw();

}


#include "shadowmap.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "shadowmap_vshader.h"
#include "shadowmap_fshader.h"


Shadowmap::Shadowmap(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


GLuint Shadowmap::init(Vertices* vertices, GLuint heightMapTexID) {

    /// Common initialization.
    preinit(vertices, shadowmap_vshader, shadowmap_fshader, NULL, "vertexPosition2DModel");

    /// Bind the heightmap to texture 0.
    set_texture(0, heightMapTexID, "heightMapTex", GL_TEXTURE_2D);

    /// Create and bind the texture which will contain the
    /// depth output (the actual shadow map) of our shader. A depth texture is
    /// slower than a depth buffer, but can be sampled later.
    GLuint shadowMapTexID;
    glGenTextures(1, &shadowMapTexID);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexID);

    /// Depth format is unsigned integer, not float. Set the number of bits.
    // 16 bits : GL_DEPTH_COMPONENT16 & GL_UNSIGNED_SHORT
    // 32 bits : GL_DEPTH_COMPONENT32 & GL_UNSIGNED_INT
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

    /// Linear interpolation.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /// Clamp texture coordinates to the [0,1] range.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /// Texture comparison mode and operator for percentage closer filtering (PCF).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    /// Attach the created texture to the depth attachment point.
    /// Hardware will copy pixel depth to the texture.
    glGenFramebuffers(1, &_framebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTexID, 0);

    /// Disable reads and writes from / to the color buffer as there is none.
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadowmap framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Set uniform IDs.
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");

    /// Return the shadowmap texture ID (for the terrain).
    return shadowMapTexID;

}


void Shadowmap::draw(const mat4& lightViewProjection) const {

    /// Common drawing.
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv( _lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());

    /// Render from light source point of view to FBO.
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);
    glClear(GL_DEPTH_BUFFER_BIT);
    _vertices->draw();

}

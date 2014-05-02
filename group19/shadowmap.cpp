
#include "shadowmap.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "shadowmap_vshader.h"
#include "shadowmap_fshader.h"


/// Shadow map texture size.
const int texWidth(1024);
const int texHeight(1024);


void Shadowmap::init(GLuint heightMapTexID) {

    // Common initialization.
    RenderingContext::init(shadowmap_vshader, shadowmap_fshader);

    /// Create a framebuffer (container for textures, and an optional depth buffer).
    /// The shadow map will be rendered to this FBO instead of the screen.
    /// Specify the transformation from normalized device coordinates to texture coordinates.
    glGenFramebuffers(1, &_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
    glViewport(0, 0, texWidth, texHeight);

    /// Bind the heightmap to texture 0.
    const GLuint heightMapTex = 0;
    glActiveTexture(GL_TEXTURE0+heightMapTex);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
    GLuint uniformID = glGetUniformLocation(_programID, "heightMapTex");
    glUniform1i(uniformID, heightMapTex);

    /// Create the texture which will contain the color / depth output
    /// (the actual shadow map) of our shader.
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &_shadowMapTexID);
    glBindTexture(GL_TEXTURE_2D, _shadowMapTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, texWidth, texWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    /// Configure the framebuffer : shadowmapTexture become the
    /// fragment shader first output buffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowMapTexID, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadowmap framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

}


void Shadowmap::draw(mat4& projection, mat4& modelview) const {

    // Common drawing.
    RenderingContext::draw();

    /// The shadow map will be rendered to this FBO instead of the screen.
    /// Specify the transformation from normalized device coordinates to texture coordinates.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
    glViewport(0, 0, texWidth, texHeight);


    //--- Bind the necessary textures

    //--- Update the content of the uniforms (texture IDs, matrices, ...)

    //--- Render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindVertexArray(0);

}


void Shadowmap::clean() {
    RenderingContext::clean();
}

#include "vertices.h"
#include "waterReflection.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "watereflection_vshader.h"
#include "watereflection_fshader.h"



WaterReflection::WaterReflection(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}

GLuint WaterReflection::init(Vertices* vertices, GLuint heightMapTexID) {

	/// Common initialization.
    RenderingContext::init(vertices, watereflection_vshader, watereflection_fshader, "vertexPosition2DModel", -1);

	/// Bind the heightmap to texture 0.
    set_texture(0, heightMapTexID, "heightMapTex", GL_TEXTURE_2D);

	 /// Create and bind to texture 1 the texture which will contain the
    // scene viewed from reflected camera
	GLuint reflectionID;
    glGenTextures(1, &reflectionID);
    glBindTexture(GL_TEXTURE_2D, reflectionID);

	/// Empty image (no data), one color component, unclamped 32 bits float.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /// Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /// Attach the created texture to the depth attachment point.
    /// Hardware will copy pixel depth to the texture.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectionID, 0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Reflection texture framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Set uniform IDs.
    _lightMatrixID = glGetUniformLocation(_programID, "lightMVP");

    /// Return the reflection texture ID
    return reflectionID;
}

void WaterReflection::draw(const mat4& lightMVP) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_lightMatrixID, 1, GL_FALSE, lightMVP.data());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from light source point of view to FBO.
    _vertices->draw(_vertexAttribID);

}
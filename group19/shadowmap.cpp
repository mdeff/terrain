
#include "shadowmap.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "shadowmap_vshader.h"
#include "shadowmap_fshader.h"

// FIXME : vertices as an object ?
extern unsigned int nIndices;


Shadowmap::Shadowmap(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void Shadowmap::init(GLuint heightMapTexID, GLint vertexArrayID) {

    /// Common initialization : vertex array and shader programs.
    RenderingContext::init(shadowmap_vshader, shadowmap_fshader, -1, vertexArrayID);

    /// Bind the heightmap to texture 0.
    set_texture(0, heightMapTexID, "heightMapTex");

    /// Create the texture which will contain the color / depth output
    /// (the actual shadow map) of our shader.
    // Depth texture. Slower than a depth buffer, but we can sample it later in the shader.
    set_texture(1);

    // Depth format is unsigned int. Set it to 16 bits.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    /// Attach the created texture to the depth attachment point.
    /// The texture becomes the fragment shader first output buffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _textureIDs[1], 0);

    /// There is only depth (no color) output in the bound framebuffer.
    //glDrawBuffer(GL_NONE);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadowmap framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }


    /// Vertex attribute "position" points to data from the currently binded array buffer.
    //    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    GLuint positionID = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(positionID);
    // vec2: 2 floats per vertex for the xy plane position attribute.
    glVertexAttribPointer(positionID, 2, GL_FLOAT, GL_FALSE, 0, 0);


    /// Set uniform and attribute IDs.
    _lightMatrixID = glGetUniformLocation(_programID, "lightMVP");
    _vertexAttribID = glGetAttribLocation(_programID, "position");

}


void Shadowmap::draw(mat4& /*projection*/, mat4& /*modelview*/) const {

    // Common drawing.
    RenderingContext::draw();

    /// Vertex attribute "position" points to data from the currently binded array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    glEnableVertexAttribArray(_vertexAttribID);
    glVertexAttribPointer(_vertexAttribID, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //--- Update the content of the uniforms (texture IDs, matrices, ...)

    /// Spot light projection.
    float fieldOfView = 45.0f;
    float aspectRatio = 1.f;
    float nearPlane = 0.1f;
    float farPlane  = 10.f;
    static mat4 projection = Eigen::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

    /// Light position.
    vec3 lightPosition(3.0, 3.0, 3.0);
    vec3 lightAt(0.0,0.0,0.0);
    //vec3 lightUp(0.0,1.0,0.0);
    vec3 lightUp(0.0,0.0,1.0);
    static mat4 view = Eigen::lookAt(lightPosition, lightAt, lightUp);

    /// Assemble the lightMVP matrix for a spotlight source.
    mat4 lightMVP = projection * view;
    GLuint lightMatrixID = glGetUniformLocation(_programID, "lightMVP");
    glUniformMatrix4fv(lightMatrixID, 1, GL_FALSE, lightMVP.data());



    /// Clear the framebuffer object.
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from light source point of view to FBO.
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(_vertexAttribID);
}


void Shadowmap::clean() {
    RenderingContext::clean();
}


#include "rendering_context.h"
#include "opengp.h"


RenderingContext::RenderingContext(unsigned int width, unsigned int height) :
    _width(width), _height(height) {
}


void RenderingContext::init(const char* vshader, const char* fshader) {

    /// Vertex array object.
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Compile and install the rendering shaders.
    _programID = opengp::compile_shaders(vshader, fshader, 0, 0, 0);
    if(!_programID)
        exit(EXIT_FAILURE);
    glUseProgram(_programID);

}


void RenderingContext::draw() const {

    /// Set the object FBO as the rendering target.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);

    /// Specify the transformation from normalized device coordinates
    /// to texture/window coordinates.
    glViewport(0, 0, _width, _height);

    /// Select the shader program.
    glUseProgram(_programID);

    /*
     * Bind vertex array
     * A vertex array object holds references to the vertex buffers, the index
     * buffer and the layout specification of the vertex itself. At runtime,
     * you can just glBindVertexArray to recall all of these information.
     */
    glBindVertexArray(_vertexArrayID);

}


void RenderingContext::clean() {

    glDeleteProgram(_programID);
    glDeleteVertexArrays(1, &_vertexArrayID);

}

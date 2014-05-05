
#include <string>
#include "rendering_context.h"
#include "opengp.h"


/// By default,render to the default framebuffer (screen) : FBO 0.
RenderingContext::RenderingContext(unsigned int width, unsigned int height) :
    _width(width), _height(height), _nTextures(0) {
}


void RenderingContext::init(const char* vshader, const char* fshader, GLint frameBufferID, GLint vertexArrayID) {

    /// Create a framebuffer (container for textures, and an optional depth buffer).
    /// The created FBO (instead of the screen) will contain the rendering results.
    if(frameBufferID < 0)
        glGenFramebuffers(1, (GLuint*)&frameBufferID);
    _frameBufferID = frameBufferID;

    /// Set the context FBO as the rendering target.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);

    /// Vertex array object.
    if(vertexArrayID < 0)
        glGenVertexArrays(1, (GLuint*)&vertexArrayID);
    _vertexArrayID = vertexArrayID;
    glBindVertexArray(_vertexArrayID);

    /// Compile and install the rendering shaders.
    _programID = opengp::compile_shaders(vshader, fshader, 0, 0, 0);
    if(!_programID)
        exit(EXIT_FAILURE);
    glUseProgram(_programID);

}


void RenderingContext::draw() const {

    /// Set the context FBO as the rendering target.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);

    /// Specify the transformation from normalized device coordinates
    /// to texture/window coordinates.
    glViewport(0, 0, _width, _height);

    /// Select the shader program.
    glUseProgram(_programID);

    /// Bind all the necessary textures.
    for(int i=0; i<_nTextures; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _textureIDs[i]);
    }

    /*
     * Bind vertex array
     * A vertex array object holds references to the vertex buffers, the index
     * buffer and the layout specification of the vertex itself. At runtime,
     * you can just glBindVertexArray to recall all of these information.
     */
    glBindVertexArray(_vertexArrayID);

}


void RenderingContext::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteBuffers(1, &_elementBufferID);
    glDeleteProgram(_programID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}


GLuint RenderingContext::set_texture(const GLuint textureIndex, int textureID, std::string uniformName, GLenum target) {

    /// Create a texture if no ID was passed.
    if(textureID < 0)
        glGenTextures(1, (GLuint*)&textureID);

    /// "Bind" the newly created texture to the context : all future texture functions will modify this texture.
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(target, textureID);

    if(!uniformName.empty()) {
        GLuint uniformID = glGetUniformLocation(_programID, uniformName.c_str());
        glUniform1i(uniformID, textureIndex);
    }

    _textureIDs[textureIndex] = textureID;
    _nTextures = textureIndex + 1;

    return textureID;
}


GLuint RenderingContext::get_texture_ID(const GLuint textureIndex) const {
    return _textureIDs[textureIndex];
}

void RenderingContext::set_texture_ID(const GLuint textureIndex, const GLuint textureID) {
    _textureIDs[textureIndex] = textureID;
    _nTextures = textureIndex + 1;
}


void RenderingContext::get_buffer_IDs(GLuint& vertexBufferID, GLuint& elementBufferID) const {
    vertexBufferID = _vertexBufferID;
    elementBufferID = _elementBufferID;
}

void RenderingContext::set_buffer_IDs(const GLuint vertexBufferID, const GLuint elementBufferID) {
    _vertexBufferID = vertexBufferID;
    _elementBufferID = elementBufferID;
}


GLuint RenderingContext::get_vertexarray_ID() const {
    return _vertexArrayID;
}

void RenderingContext::set_vertexarray_ID(const GLuint vertexArrayID) {
    _vertexArrayID = vertexArrayID;
}

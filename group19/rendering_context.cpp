
#include <string>
#include "rendering_context.h"
#include "vertices.h"
#include "opengp.h"


/// By default,render to the default framebuffer (screen) : FBO 0.
RenderingContext::RenderingContext(unsigned int width, unsigned int height) :
    _width(width), _height(height) {
    for(int i = 0; i<_nTextures; i++)
        _textureIDs[i] = -1;
}


void RenderingContext::init(Vertices* vertices, const char* vshader, const char* fshader, GLint frameBufferID) {

    _vertices = vertices;

    /// Create a framebuffer (container for textures, and an optional depth buffer).
    /// The created FBO (instead of the screen) will contain the rendering results.
    if(frameBufferID < 0)
        glGenFramebuffers(1, (GLuint*)&frameBufferID);
    _frameBufferID = frameBufferID;

    /// Set the context FBO as the rendering target.
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);

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
        if(_textureIDs[i] >= 0) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, (GLuint)_textureIDs[i]);
        }
    }
}


void RenderingContext::clean() {
    _vertices->clean();
    glDeleteProgram(_programID);
    /// Do not delete the default (screen) framebuffer.
    if(_frameBufferID != 0)
        glDeleteFramebuffers(1, &_frameBufferID);
}


GLuint RenderingContext::set_texture(const GLuint textureIndex, int textureID, std::string uniformName, GLenum target) {

    /// Create a texture if no ID was passed.
    if(textureID < 0)
        glGenTextures(1, (GLuint*)&textureID);

    /// Bind the newly created texture to the context :
    /// all future texture functions will modify this texture.
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(target, textureID);

    if(!uniformName.empty()) {
        GLuint uniformID = glGetUniformLocation(_programID, uniformName.c_str());
        glUniform1i(uniformID, textureIndex);
    }

    _textureIDs[textureIndex] = textureID;

    return textureID;
}

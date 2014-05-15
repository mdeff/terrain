
#include "rendering_context.h"
#include "vertices.h"
#include <GL/glew.h>
#include "opengp.h"


/// By default,render to the default framebuffer (screen) : FBO 0.
RenderingContext::RenderingContext(unsigned int width, unsigned int height) :
    _width(width), _height(height) {
    for(int i = 0; i<_nTextures; ++i)
        _textures[i].ID = -1;
}


void RenderingContext::init(Vertices* vertices, const char* vshader, const char* fshader, const char* vertexAttribName, GLint frameBufferID) {

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

    /// Set vertex attribute array ID.
    _vertexAttribID = glGetAttribLocation(_programID, vertexAttribName);

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
        if(_textures[i].ID >= 0) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(_textures[i].target, (GLuint)_textures[i].ID);
        }
    }
}


void RenderingContext::clean() {

    /// Delete the shader programs.
    glDeleteProgram(_programID);

    /// Do not delete the default (screen) framebuffer.
    if(_frameBufferID != 0)
        glDeleteFramebuffers(1, &_frameBufferID);

    /// Delete all the binded textures.
    // TODO: be carefull of shared textures.
    for(int i=0; i<_nTextures; ++i) {
        if(_textures[i].ID >= 0) {
            glDeleteTextures(1, (GLuint*)&_textures[i].ID);
        }
    }
}


void RenderingContext::set_texture(const GLuint textureIndex, int textureID, const char* uniformName, GLenum target) {

    /// Create a texture if no ID was passed.
    if(textureID < 0)
        glGenTextures(1, (GLuint*)&textureID);

    /// Bind the newly created texture to the context :
    /// all future texture functions will modify this texture.
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(target, textureID);

    /// Put the texture index value in the Sampler uniform.
    GLuint uniformID = glGetUniformLocation(_programID, uniformName);
    glUniform1i(uniformID, textureIndex);

    _textures[textureIndex].ID = textureID;
    _textures[textureIndex].target = target;

}

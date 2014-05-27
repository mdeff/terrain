
#include "rendering_context.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"


RenderingContext::RenderingContext(unsigned int width, unsigned int height) :
    _width(width), _height(height) {
    for(int i = 0; i<_nTextures; ++i)
        _textures[i].ID = -1;
}


void RenderingContext::preinit(Vertices* vertices, const char* vshader, const char* fshader, const char* gshader, const char* vertexAttribName1, const char* vertexAttribName2) {

    _vertices = vertices;

    /// Compile and install the rendering shaders.
    _programID = opengp::compile_shaders(vshader, fshader, gshader, NULL, NULL);
    if(!_programID)
        exit(EXIT_FAILURE);

    /// Needed for glUniform* as glProgramUniform* does not work on AMD/ATI.
    /// Direct state access (DSA) was only lately implemented by them.
    glUseProgram(_programID);

    /// Bind the vertex attribute ID to vertex data, if they exist.
    if(vertices != NULL && vertexAttribName1 != NULL) {
        GLuint vertexAttribID1 = glGetAttribLocation(_programID, vertexAttribName1);
        _vertices->bind(vertexAttribID1);
    }

	if(vertices != NULL && vertexAttribName2 != NULL) {
        GLuint vertexAttribID2 = glGetAttribLocation(_programID, vertexAttribName2);
        _vertices->bind(vertexAttribID2);
    }
}


void RenderingContext::predraw() const {

    /// Select the shader program.
    glUseProgram(_programID);

    /// Bind all the necessary textures.
    for(int i=0; i<_nTextures; ++i) {
        if(_textures[i].ID >= 0) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(_textures[i].target, (GLuint)_textures[i].ID);
        }
    }

    /// Specify the transformation from normalized device coordinates
    /// to texture/window coordinates.
    glViewport(0, 0, _width, _height);

}


void RenderingContext::preclean() {

    /// Delete the shader programs.
    glDeleteProgram(_programID);

    /// Delete all the binded textures.
    // TODO: take care of shared textures.
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
    glBindTexture(target, textureID);

    /// Put the texture index value in the Sampler uniform.
    GLuint uniformID = glGetUniformLocation(_programID, uniformName);
    glUniform1i(uniformID, textureIndex);

    _textures[textureIndex].ID = textureID;
    _textures[textureIndex].target = target;

}


void RenderingContext::load_texture(const char * imagepath) const {

    /// Read the file.
    if(glfwLoadTexture2D(imagepath, 0)) {

        /// We want to repeat the texture for texture and normal mapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Nice trilinear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

    } else {
        std::cout << "Cannot load texture file : " << imagepath << std::endl;
        exit(EXIT_FAILURE);
    }

}

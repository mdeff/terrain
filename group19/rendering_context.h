
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <string>
#include <GL/glew.h>


/// @brief OpenGL wrapper class template
class RenderingContext {

public:

    GLuint set_texture(const GLuint textureIndex, GLint textureID = -1, std::string uniformName = "", GLenum target = GL_TEXTURE_2D);

    void get_buffer_IDs(GLuint& vertexBufferID, GLuint& elementBufferID) const;
    void set_buffer_IDs(const GLuint vertexBufferID, const GLuint elementBufferID);

    GLuint get_vertexarray_ID() const;
    void set_vertexarray_ID(const GLuint vertexArrayID);

    GLuint get_texture_ID(const GLuint textureIndex) const;
    void set_texture_ID(const GLuint textureIndex, const GLuint textureID);

protected:

    // Common methods of all renderers.
    RenderingContext(unsigned int width, unsigned int height);
    void init(const char* vshader, const char* fshader, GLint frameBufferID = 0, GLint vertexArrayID = -1);  ///< Or in the constructor
    void draw() const;
    void clean();  ///< Or in the destructor

    // Common properties of all renderers.
    GLuint _programID;
    GLuint _frameBufferID;
    unsigned int _width, _height;

    GLuint _vertexAttribID;
    GLuint _vertexBufferID;
    GLuint _elementBufferID;

    static const int _nTextures = 20;
    GLint _textureIDs[_nTextures];


private:

    GLuint _vertexArrayID;


};

#endif /* __rendering_context_h__ */

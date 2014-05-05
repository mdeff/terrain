
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <string>
#include <GL/glew.h>


/// @brief OpenGL wrapper class template
class RenderingContext {

public:

    GLuint set_texture(const GLuint textureIndex, int textureID, std::string uniformName = "", GLenum target = GL_TEXTURE_2D);

    void get_buffer_IDs(GLuint& vertexBufferID, GLuint& elementBufferID) const;
    void set_buffer_IDs(const GLuint vertexBufferID, const GLuint elementBufferID);

    GLuint get_vertexarray_ID() const;
    void set_vertexarray_ID(const GLuint vertexArrayID);

protected:

    // Common methods of all renderers.
    RenderingContext(unsigned int width, unsigned int height);
    void init(const char* vshader, const char* fshader);  ///< Or in the constructor
    void draw() const;
    void clean();  ///< Or in the destructor

    // Common properties of all renderers.
    GLuint _programID;
    GLuint _frameBufferID;
    unsigned int _width, _height;

    GLuint _vertexBufferID;
    GLuint _elementBufferID;

private:

    GLuint _vertexArrayID;

    GLuint _nTextures;
    GLuint _textureIDs[20];

};

#endif /* __rendering_context_h__ */

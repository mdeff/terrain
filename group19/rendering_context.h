
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <string>
#include <GL/glew.h>

// Forward declarations (no header includes) (namespace pollution, build time).
class Vertices;

/// @brief OpenGL wrapper class template
class RenderingContext {

public:

protected:

    unsigned int set_texture(unsigned int textureIndex, int textureID = -1, std::string uniformName = "", unsigned int target = GL_TEXTURE_2D);

    // Common methods of all renderers.
    RenderingContext(unsigned int width, unsigned int height);
    void init(Vertices* vertices, const char* vshader, const char* fshader, int frameBufferID = 0);
    void draw() const;
    void clean();

    // Common properties of all renderers.
    unsigned int _programID;
    unsigned int _frameBufferID;
    unsigned int _width, _height;

    unsigned int _vertexAttribID;

    static const int _nTextures = 20;
    unsigned int _textureIDs[_nTextures];

    Vertices* _vertices;

private:



};

#endif /* __rendering_context_h__ */

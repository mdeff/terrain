
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <string>
#include <GL/glew.h>

/// Forward declarations (no header includes) (namespace pollution, build time).
class Vertices;

/// @brief OpenGL wrapper class template
class RenderingContext {

public:

    /// Common methods of all RenderingContext.
    void clean();

protected:

    /// Common methods of all RenderingContext.
    RenderingContext(unsigned int width, unsigned int height);
    void init(Vertices* vertices, const char* vshader, const char* fshader, const char* vertexAttribName, int frameBufferID = 0);
    void draw() const;

    /// Helper function.
    unsigned int set_texture(unsigned int textureIndex, int textureID = -1, std::string uniformName = "", unsigned int target = GL_TEXTURE_2D);

    /// Common properties of all RenderingContext.
    unsigned int _width, _height;
    unsigned int _programID;
    unsigned int _vertexAttribID;

    /// Pointer : vertices object can be shared between RenderingContext.
    Vertices* _vertices;

private:

    /// Common properties of all RenderingContext.
    unsigned int _frameBufferID;

    /// Textures.
    static const int _nTextures = 20;
    unsigned int _textureIDs[_nTextures];

};

#endif /* __rendering_context_h__ */

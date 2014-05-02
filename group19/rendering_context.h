
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <GL/glew.h>

/// @brief OpenGL wrapper class template
class RenderingContext {

protected:

    // Common methods of all renderers.
    void init(const char* vshader, const char* fshader);  ///< Or in the constructor
    void draw() const;
    void clean();  ///< Or in the destructor

    // Common properties of all renderers.
    GLuint _vertexArrayID;
    GLuint _programID;

};

#endif /* __rendering_context_h__ */

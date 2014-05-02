#include <GL/glew.h>
#include "opengp.h"

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

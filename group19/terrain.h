
#include "rendering_context.h"

#include <GL/glew.h>
#include "opengp.h"

/// @brief OpenGL wrapper class template
class Terrain : public RenderingContext {

public:
    // Common methods of all renderers.
    void init(GLuint heightMapTexID);  ///< Or in the constructor
    void draw(mat4& projection, mat4& model_view) const;
    void clean();  ///< Or in the destructor

private:
    // Specialized to this object.
    GLuint _modelviewID;
    GLuint _projectionID;
    GLuint _timerID;

    void gen_triangle_grid();
    GLuint loadTexture(const char * imagepath);

};

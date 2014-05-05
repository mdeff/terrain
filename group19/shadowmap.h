
#ifndef __shadowmap_h__
#define __shadowmap_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Shadowmap : public RenderingContext {

public:
    // Common methods of all renderers.
    Shadowmap(unsigned int width, unsigned int height);
    void init(GLuint heightMapTexID, GLint vertexArrayID);  ///< Or in the constructor
    void draw(mat4& projection, mat4& model_view) const;
    void clean();  ///< Or in the destructor

private:
    // Specialized to this object.
    GLuint _shadowMapTexID;

    GLuint _lightMatrixID;

    void gen_triangle_grid();
    GLuint loadTexture(const char * imagepath);

};

#endif /* __shadowmap_h__ */

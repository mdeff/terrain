
#ifndef __terrain_h__
#define __terrain_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Terrain : public RenderingContext {

public:
    // Common methods of all renderers.
    Terrain(unsigned int width, unsigned int height);
    void init(GLuint heightMapTexID);  ///< Or in the constructor
    void draw(mat4& projection, mat4& modelview, mat4& lightMVP) const;
    void clean();  ///< Or in the destructor

private:
    // Specialized to this object.
    GLuint _modelviewID;
    GLuint _projectionID;
    GLuint _timeID;
    GLuint _lightOffsetMVPID;

    void gen_triangle_grid();
    GLuint load_texture(const char * imagepath) const;

};

#endif /* __terrain_h__ */

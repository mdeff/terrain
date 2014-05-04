
#ifndef __watermap_h__
#define __watermap_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Watermap : public RenderingContext {

public:
    // Common methods of all renderers.
    Watermap(unsigned int width, unsigned int height);
    void init(GLuint heightMapTexID);  ///< Or in the constructor
    void draw(mat4& projection, mat4& model_view) const;
    void clean();  ///< Or in the destructor

private:
    // Specialized to this object.
    GLuint _waterMapTexID;

    GLuint _modelviewID;
    GLuint _projectionID;
	GLuint loadCubeTexture(int slotNum) const;
    void gen_triangle_grid();
    GLuint loadTexture(const char * imagepath);

};

#endif /* __watermap_h__ */
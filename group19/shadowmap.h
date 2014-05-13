
#ifndef __shadowmap_h__
#define __shadowmap_h__

#include "rendering_context.h"
#include "opengp.h"

class Vertices;

class Shadowmap : public RenderingContext {

public:
    // Common methods of all renderers.
    Shadowmap(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices, unsigned int heightMapTexID);
    void draw(mat4& projection, mat4& model_view, mat4& lightMVP) const;
    void clean();

private:
    // Specialized to this object.
    unsigned int _shadowMapTexID;

    unsigned int _lightMatrixID;

    void gen_triangle_grid();
    unsigned int loadTexture(const char * imagepath);

};

#endif /* __shadowmap_h__ */

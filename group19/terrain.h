
#ifndef __terrain_h__
#define __terrain_h__

#include "rendering_context.h"
#include "opengp.h"

class Vertices;

class Terrain : public RenderingContext {

public:
    // Common methods of all renderers.
    Terrain(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int heightMapTexID, unsigned int shadowMapTexID);
    void draw(mat4& projection, mat4& modelview, mat4& lightMVP, vec3& lightPositionModel) const;
    void clean();

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;
    unsigned int _lightOffsetMVPID;
    unsigned int _lightPositionModelID;
    unsigned int _timeID;

    void gen_triangle_grid();
    unsigned int load_texture(const char * imagepath) const;

};

#endif /* __terrain_h__ */

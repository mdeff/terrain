
#ifndef __terrain_h__
#define __terrain_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Terrain : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Terrain(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int heightMapTexID, unsigned int shadowMapTexID);
    void draw(const mat4& projection, const mat4& modelview,
              const mat4& lightMVP, const vec3& lightPositionModel) const;

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;
    unsigned int _lightOffsetMVPID;
    unsigned int _lightPositionModelID;
    unsigned int _timeID;

    /// Helper function.
    unsigned int load_texture(const char * imagepath) const;

};

#endif /* __terrain_h__ */

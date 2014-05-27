
#ifndef __watermap_h__
#define __watermap_h__

#include "rendering_context.h"

#include <GL/glew.h>
#include "opengp.h"

class Watermap : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Watermap(unsigned int width, unsigned int height);
    //void init(Vertices* vertices, unsigned int heightMapTexID, unsigned int shadowMapTexID);
    void init(Vertices* vertices, GLuint heightMapTexID);
    void draw(const mat4& projection, const mat4& view,
              const mat4& lightViewProjection, const vec3& lightPositionWorld) const;

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;
    unsigned int _lightViewProjectionID;
    unsigned int _lightPositionWorldID;
    unsigned int _timeID;

};

#endif /* __terrain_h__ */


#ifndef __terrain_h__
#define __terrain_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Terrain : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Terrain(unsigned int windowWidth, unsigned int windowHeight);
    unsigned int init(Vertices* vertices, unsigned int framebufferIDs[], unsigned int heightMapTexID, unsigned int shadowMapTexID, unsigned int& reflectionFramebufferID);
    void draw(const mat4& projection, const mat4& view,
              const mat4& lightViewProjection, const vec3& lightPositionWorld) const;

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;
    unsigned int _lightViewProjectionID;
    unsigned int _lightPositionWorldID;
    unsigned int _clipID;
    unsigned int _seedID;

};

#endif /* __terrain_h__ */

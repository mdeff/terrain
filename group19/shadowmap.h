
#ifndef __shadowmap_h__
#define __shadowmap_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Shadowmap : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Shadowmap(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices, unsigned int heightMapTexID);
    void draw(const mat4& lightViewProjection) const;

private:

    /// Framebuffer object.
    unsigned int _framebufferID;

    /// Uniform IDs.
    unsigned int _lightViewProjectionID;

};

#endif /* __shadowmap_h__ */


#ifndef __particles_render_h__
#define __particles_render_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class ParticlesRender : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesRender(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int particlesPosTexID);
    void draw(const mat4& projection, const mat4& modelview) const;

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;

};

#endif /* __particles_render_h__ */

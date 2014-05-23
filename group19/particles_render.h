
#ifndef __particles_render_h__
#define __particles_render_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class ParticlesRender : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesRender(unsigned int width, unsigned int height);
    void init(unsigned int particlePosTexID[]);
    void draw(const mat4& projection, const mat4& modelview);

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;

    /// Texture IDs : current and last particle positions.
    unsigned int _particlePosTexID[2];

};

#endif /* __particles_render_h__ */

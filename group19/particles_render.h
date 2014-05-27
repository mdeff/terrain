
#ifndef __particles_render_h__
#define __particles_render_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class ParticlesRender : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesRender(unsigned int width, unsigned int height, unsigned int nParticlesSide);
    void init(unsigned int particlePosTexID[]);
    void draw(const mat4& projection, const mat4 views[]);

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;

    /// Texture IDs : current and last particle positions.
    unsigned int _particlePosTexID[2];

    /// Number of particles on the side. That makes nParticlesSide^3 particles.
    const unsigned int _nParticlesSide;

};

#endif /* __particles_render_h__ */

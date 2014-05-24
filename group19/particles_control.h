
#ifndef __particles_control_h__
#define __particles_control_h__

#include "rendering_context.h"

class ParticlesControl : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesControl(unsigned int nParticlesSide);
    void init(Vertices* vertices, unsigned int particlePosTexID[]);
    void draw() const;

private:

    /// Uniform IDs.
    unsigned int _deltaTID;

    /// Texture IDs : current and last particles position and velocity.
    unsigned int _particleTexID[4];

    /// Number of particles on the side. That makes nParticlesSide^3 particles.
    const unsigned int _nParticlesSide;

};

#endif /* __particles_control_h__ */

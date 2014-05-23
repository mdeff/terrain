
#ifndef __particles_control_h__
#define __particles_control_h__

#include "rendering_context.h"

class ParticlesControl : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesControl();
    void init(Vertices* vertices, unsigned int particlePosTexID[]);
    void draw() const;

private:

    /// Uniform IDs.
    unsigned int _deltaTID;

    /// Texture IDs : current and last particles position and velocity.
    unsigned int _particleTexID[4];

    /// Helper functions.

};

#endif /* __particles_control_h__ */

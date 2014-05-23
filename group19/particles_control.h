
#ifndef __particles_control_h__
#define __particles_control_h__

#include "rendering_context.h"

class ParticlesControl : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ParticlesControl(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices);
    void draw() const;

private:

    /// Uniform IDs.

    /// Helper functions.

};

#endif /* __particles_control_h__ */


#ifndef __heightmap_h__
#define __heightmap_h__

#include "rendering_context.h"

class Heightmap : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Heightmap(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices);
    void draw() const;

    /// Generate a fake heightmap.
    unsigned int test() const;

private:

    /// Uniform IDs.

    /// Helper functions.
    unsigned int gen_permutation_table() const;
    unsigned int gen_gradient_vectors() const;

};

#endif /* __heightmap_h__ */

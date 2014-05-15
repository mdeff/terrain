
#ifndef __shadowmap_h__
#define __shadowmap_h__

#include "rendering_context.h"
#include "opengp.h"

class Shadowmap : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Shadowmap(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices, unsigned int heightMapTexID);
    void draw(const mat4& lightMVP) const;

private:

    /// Uniform IDs.
    unsigned int _lightMatrixID;

};

#endif /* __shadowmap_h__ */

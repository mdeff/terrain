
#ifndef __rendering_simple_h__
#define __rendering_simple_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class RenderingSimple : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    RenderingSimple(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4& view, const mat4& model, const vec3& color) const;

private:

    /// Uniform IDs.
    unsigned int _projectionID;
    unsigned int _viewID;
    unsigned int _modelID;
    unsigned int _colorID;

};

#endif /* __rendering_simple_h__ */


#ifndef __camera_pictorial_h__
#define __camera_pictorial_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class CameraPictorial : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    CameraPictorial(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4& view, const mat4& model) const;

private:

    /// Uniform IDs.
    unsigned int _projectionID;
    unsigned int _viewID;
    unsigned int _modelID;

};

#endif /* __camera_pictorial_h__ */

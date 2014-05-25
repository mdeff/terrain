
#ifndef __camera_path_h__
#define __camera_path_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class CameraPath : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    CameraPath(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4& view) const;

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;

};

#endif /* __camera_path_h__ */

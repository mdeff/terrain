
#ifndef __camera_path_control_points_h__
#define __camera_path_control_points_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class CameraPathControlPoints : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    CameraPathControlPoints(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4& view, const int& selectedControlPoint) const;

private:

    /// Uniform IDs.
    unsigned int _projectionID;
    unsigned int _viewID;
    unsigned int _selectedControlPointID;

};

#endif /* __camera_path_control_points_h__ */

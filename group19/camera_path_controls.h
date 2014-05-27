
#ifndef __camera_path_controls_h__
#define __camera_path_controls_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class CameraPathControls : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    CameraPathControls(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4 views[], const vec3& lightPositionWorld, const int& selectedControlPoint, float deltaT) const;

private:

    /// Uniform IDs.
    unsigned int _projectionID;
    unsigned int _viewID;
    unsigned int _lightPositionWorldID;
    unsigned int _selectedControlPointID;
    unsigned int _rotationMatrixID;

};

#endif /* __camera_path_controls_h__ */

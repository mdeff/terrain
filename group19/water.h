
#ifndef __water_h__
#define __water_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Water : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Water(unsigned int width, unsigned int height);
    //void init(Vertices* vertices, unsigned int heightMapTexID, unsigned int shadowMapTexID);
    void init(Vertices* vertices, unsigned int renderedTexIDs[]);
    void draw(const mat4& projection, const mat4 views[],
              const mat4& lightViewProjection, const vec3& lightPositionWorld) const;

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;
    unsigned int _lightViewProjectionID;
    unsigned int _lightPositionWorldID;
    unsigned int _timeID;

    /// Texture IDs : reflection from 2 view points.
    unsigned int _reflectionTexID[2];

};

#endif /* __water_h__ */

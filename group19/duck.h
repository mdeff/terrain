#ifndef _duck_h_
#define _duck_h_

#include <iostream>
#include <vector>

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class RenderedDuck : public RenderingContext{

public:

    /// Common methods of all RenderingContext.
	RenderedDuck(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4 views[], const vec3& lightPositionWorld);

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;
	unsigned int _transID;
	unsigned int _lightPositionWorldID;
	unsigned int _aniMat;


};

#endif /* _duck_h_ */

#ifndef __waterReflection_h__
#define __waterReflection_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class WaterReflection : public RenderingContext {

public:
	WaterReflection(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices, unsigned int heightMapTexID);
    void draw(const mat4& lightMVP) const;

private:
	 /// Uniform IDs.
    unsigned int _lightMatrixID;

};

#endif
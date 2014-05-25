#ifndef __waterReflection_h__
#define __waterReflection_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"


#define GROUND_HEIGHT 0.018f

class WaterReflection : public RenderingContext {

public:
	WaterReflection(unsigned int width, unsigned int height);
    unsigned int init(Vertices* vertices, unsigned int heightMapTexID);
    void draw(const mat4& projection, const mat4& modelview,
                   const mat4& lightMVP, const vec3& lightPositionModel) const;

private:
	 /// Uniform IDs.
    unsigned int _lightMatrixID;
	 /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;
    unsigned int _lightOffsetMVPID;
    unsigned int _lightPositionModelID;
	unsigned int _lightMVPID;

	//helper function
    GLuint load_texture(const char * imagepath) const;

};

#endif

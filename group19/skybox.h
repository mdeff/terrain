
#ifndef __skybox_h__
#define __skybox_h__

#include "rendering_context.h"
#include "opengp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Skybox : public RenderingContext {

public:
    // Common methods of all renderers.
    void init();  ///< Or in the constructor
    void draw(mat4& projection, mat4& modelview) const;
    void clean();  ///< Or in the destructor

private:
	int loadBMPtex(const char * imagepath, unsigned char* data);  //load BMP image
	GLuint load_skybox_texture(int slotNum);   //load cube texture

private:
    // Specialized to this object.
    GLuint _modelviewID;
    GLuint _projectionID;
    GLuint skybox_tex;

};

#endif /* __skybox_h__ */

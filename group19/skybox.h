
#ifndef __skybox_h__
#define __skybox_h__

#include "rendering_context.h"
#include "opengp.h"

class Skybox : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Skybox(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(mat4& projection, mat4& modelview) const;

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;

    /// Helper functions.
    int loadBMP(const char * imagepath, unsigned char* data) const;
    void loadCubeTexture() const;

};

#endif /* __skybox_h__ */

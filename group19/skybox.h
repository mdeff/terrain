
#ifndef __skybox_h__
#define __skybox_h__


#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class Skybox : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Skybox(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int reflectionFramebufferID);
    void draw(const mat4& projection, const mat4& view) const;

private:

    /// Uniform IDs.
    unsigned int _viewID;
    unsigned int _projectionID;

    /// Helper functions.
    void loadCubeTexture() const;
    int loadBMP(const char* imagepath, unsigned char* data) const;

};

#endif /* __skybox_h__ */

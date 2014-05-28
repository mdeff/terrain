
#ifndef __screen_display_h__
#define __screen_display_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class ScreenDisplay : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    ScreenDisplay(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int renderedTexIDs[]);
    void draw();

private:

    /// Uniform IDs.

    /// Texture IDs : current and last particle positions.
//    unsigned int _renderedTexIDs[2];

};

#endif /* __screen_display_h__ */


#ifndef __post_processing_h__
#define __post_processing_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

class PostProcessing : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    PostProcessing(unsigned int width, unsigned int height);
    void init(Vertices* vertices, unsigned int renderedTexIDs[]);
    void draw();

private:

    /// Uniform IDs.

    /// Texture IDs : current and last particle positions.

};

#endif /* __post_processing_h__ */

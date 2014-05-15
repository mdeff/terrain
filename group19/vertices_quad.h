
#ifndef __vertices_quad_h__
#define __vertices_quad_h__

#include "vertices.h"

class VerticesQuad : public Vertices {

public:

    /// Generate the vertices.
    void generate();

    /// Draw the scene after vertex array and vertex attribute binding.
    void draw(unsigned int vertexAttribID) const;

    /// Delete the buffers.
    void clean();

};

#endif /* __vertices_quad_h__ */

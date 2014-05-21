
#ifndef __vertices_bezier_h__
#define __vertices_bezier_h__

#include "vertices.h"

class VerticesBezier : public Vertices {

public:

    /// Generate the vertices.
    void generate();

    /// Bind the vertex attribute to the VBO (retained in VAO state).
    void bind(unsigned int vertexAttribID) const;

    /// Draw the scene.
    void draw() const;

    /// Delete the buffers.
    void clean();

};

#endif /* __vertices_bezier_h__ */

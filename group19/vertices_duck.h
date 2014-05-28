#ifndef __vertices_duck_h__
#define __vertices_duck_h__

#include "vertices.h"
#include <GL/glew.h>
#include "opengp.h"

/// For mesh I/O we use OpenGP which requires a definition of fread().
#include <cstdio>
#include <OpenGP/Surface_mesh.h>

class VerticesDuck : public Vertices, public opengp::Surface_mesh {

public:

    /// Generate the vertices.
	void generate();

    /// Bind the vertex attribute to the VBO (retained in VAO state).
    void bind(unsigned int vertexAttribIDs[]) const;

    /// Draw the scene.
    void draw() const;

    /// Delete the buffers.
    void clean();

};

#endif /* __vertices_duck_h__ */

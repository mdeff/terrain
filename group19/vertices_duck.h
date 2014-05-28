#ifndef __vertices_duck_h__
#define __vertices_duck_h__

#include "vertices.h"
#include <GL/glew.h>
#include "opengp.h"
/// For mesh I/O we use OpenGP
#include <OpenGP/Surface_mesh.h>

using namespace opengp;
using namespace std;


class VerticesDuck : public Vertices, public Surface_mesh {

public:    

	void generate();

    /// Bind the vertex attribute to the VBO (retained in VAO state).
    void bind(unsigned int vertexAttribID) const;

    /// Draw the scene.
    void draw() const;

    /// Delete the buffers.
    void clean();

private:
	unsigned int nIndices; //index of the mesh
	unsigned int* indices; //buffer holding the index of the mesh
};

#endif /* __vertices_duck_h__ */

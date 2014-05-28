
#ifndef __vertices_camera_path_h__
#define __vertices_camera_path_h__

#include "vertices.h"

class VerticesCameraPath : public Vertices {

public:

    /// Generate the OpenGL objects.
    void generate();

    /// Copy vertex data to GPU memory.
    void copy(float *vertices, unsigned int nVertices);

    /// Bind the vertex attribute to the VBO (retained in VAO state).
    void bind(unsigned int vertexAttribIDs[]) const;

    /// Draw the scene.
    void draw() const;

    /// Delete the buffers.
    void clean();

};

#endif /* __vertices_camera_path_h__ */

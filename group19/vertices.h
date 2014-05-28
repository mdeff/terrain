
#ifndef __vertices_h__
#define __vertices_h__

/// Abstract class that implements the interface.

class Vertices {

public:

    /// Generate the vertices.
    virtual void generate() = 0;

    /// Bind the vertex attribute to the VBO (retained in VAO state).
    virtual void bind(unsigned int vertexAttribIDs[]) const = 0;

    /// Draw the scene.
    virtual void draw() const = 0;

    /// Delete the buffers.
    virtual void clean() = 0;

protected:

    unsigned int _nVertices;
    unsigned int _nIndices;
    unsigned int _vertexBufferIDs[5];
    unsigned int _elementBufferID;
    unsigned int _vertexArrayID;

};

#endif /* __vertices_h__ */

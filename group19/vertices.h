
#ifndef __vertices_h__
#define __vertices_h__

/// Abstract class that implement the interface.

class Vertices {

public:

    /// Generate the vertices.
    virtual void generate() = 0;

    /// Draw the scene after vertex array and vertex attribute binding.
    virtual void draw(unsigned int vertexAttribID) const = 0;

    /// Delete the buffers.
    virtual void clean() = 0;

protected:

    unsigned int _vertexBufferID;
    unsigned int _elementBufferID;
    unsigned int _vertexArrayID;

};

#endif /* __vertices_h__ */

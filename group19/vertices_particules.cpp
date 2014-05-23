#include <iostream>

#include "vertices_particules.h"
#include <cmath>
#include <GL/glew.h>


/// The number of vertices define the number of particles.
/// It should be equal to the width times the height of the particlesPosTex.
const unsigned int nVertices = 10*10;


void VerticesParticules::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Generate nVertices : they are indices of particlesPosTex.
    int vertices[nVertices];
    for(int k=0; k<nVertices; ++k) {
        vertices[k] = k;
    }

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesParticules::bind(GLuint vertexAttribID) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // 1 int per vertex for the attribute.
    glVertexAttribIPointer(vertexAttribID, 1, GL_INT, 0, 0);

}


void VerticesParticules::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Draw the camera path vertices : either points or lines.
    glDrawArrays(GL_POINTS, 0, nVertices);

}


void VerticesParticules::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

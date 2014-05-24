#include <iostream>

#include "vertices_bezier.h"
#include <cmath>
#include <GL/glew.h>


void VerticesBezier::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Create the vertex buffer (VBO) that will contain the vertices
    /// passed to the copy() method.
    glGenBuffers(1, &_vertexBufferID);

}


void VerticesBezier::copy(float *vertices, unsigned int size) {

    /// Update the number of vertices to render.
    _nVertices = size / 3;

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    /// glBufferData() recreates the datastore. glBufferSubData() would avoid
    /// to recreate the datastore but cannot deal with a variable number of
    /// vertices. GL_STATIC_DRAW prefered as GL_STREAM_DRAW as data will be
    /// updated very unfrequently.
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), vertices, GL_STATIC_DRAW);

}


void VerticesBezier::bind(GLuint vertexAttribID) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // vec3: 3 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribID, 3, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesBezier::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Draw the camera path vertices : either points or lines.
    //glDrawArrays(GL_POINTS, 0, _nVertices);
    glDrawArrays(GL_LINES, 0, _nVertices);

}


void VerticesBezier::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

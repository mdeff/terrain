
#include "vertices_quad.h"
#include <GL/glew.h>


/// Fullscreen quad : fragment shader is executed on every pixel of the texture.
const GLfloat vertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
    -1.0f,  1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
};
const unsigned int nVertices = sizeof(vertices) / sizeof(GLfloat) / 2;


/// Copy fullscreen quad vertices to GPU.
void VerticesQuad::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesQuad::bind(GLuint vertexAttribID) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // vec2: 2 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribID, 2, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesQuad::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Render the 2 triangles (6 vertices).
    glDrawArrays(GL_TRIANGLES, 0, nVertices);

}


void VerticesQuad::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

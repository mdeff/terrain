
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

    /// Vertex array object.
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesQuad::draw(GLuint vertexAttribID) const {

    /*
     * Bind vertex array
     * A vertex array object holds references to the vertex buffers, the index
     * buffer and the layout specification of the vertex itself. At runtime,
     * you can just glBindVertexArray to recall all of these information.
     */
    glBindVertexArray(_vertexArrayID);

    /// Vertex attribute points to data from the currently binded array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // vec2: 2 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribID, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /// Render the 2 triangles (6 vertices).
    glDrawArrays(GL_TRIANGLES, 0, nVertices);

    /// No need to disable the vertex attribute as the array is only pointed
    /// by one attribute.
    glDisableVertexAttribArray(vertexAttribID);

}


void VerticesQuad::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}
